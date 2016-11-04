#include <iostream>
#include <string>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <mesos/resources.hpp>
#include <mesos/scheduler.hpp>
#include <mesos/type_utils.hpp>
#include <mesos/mesos.pb.h>

using namespace mesos;
using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::string;
using std::vector;
using boost::lexical_cast;
using mesos::Resources;
using mesos::FrameworkInfo;

const int32_t CPUS_PER_TASK = 2;
const int32_t MEM_PER_TASK = 12;
const int32_t PORT_PER_TASK = 31034;

class HelloScheduler : public Scheduler
{
public:
  HelloScheduler() {}
  virtual ~HelloScheduler() {}
  virtual void registered(SchedulerDriver*, const FrameworkID&, const MasterInfo&)
  {
    std::cout << "Registered!" << endl;
  }
  virtual void reregistered(SchedulerDriver* driver, const MasterInfo& masterInfo){}
  virtual void disconnected(SchedulerDriver* driver){}
  virtual void resourceOffers(SchedulerDriver* driver, const std::vector<Offer>& offers){
    foreach (const Offer& offer, offers) {
      std::cout << "Received offer " << offer.id() << " with " << offer.resources() << endl;
    }
    foreach (const Offer& offer, offers) {
        vector<TaskInfo> tasks;
        TaskInfo task;
        task.set_name("Task " + lexical_cast<string>(offer.id()));
        task.mutable_task_id()->set_value(lexical_cast<string>(offer.id()));
        task.mutable_slave_id()->MergeFrom(offer.slave_id());
        task.mutable_command()->set_value("echo hello-world");
        usleep(500000);
        std::cout<<"Launching Task "<< "using offer" <<offer.id() <<endl;
        
        Resource* resource;
        resource = task.add_resources();
        resource->set_name("cpus");
        resource->set_type(Value::SCALAR);
        resource->mutable_scalar()->set_value(CPUS_PER_TASK);
        resource = task.add_resources();
        resource->set_name("mem");
        resource->set_type(Value::SCALAR);
        resource->mutable_scalar()->set_value(MEM_PER_TASK);
        tasks.push_back(task);
        driver->launchTasks(offer.id(), tasks);
    }
  }

virtual void offerRescinded(SchedulerDriver* driver, const OfferID& offerId){}
virtual void statusUpdate(SchedulerDriver* driver, const TaskStatus& status){}
virtual void frameworkMessage(SchedulerDriver* driver, const ExecutorID& executorId, const SlaveID& slaveId, const std::string& data){}
virtual void slaveLost(SchedulerDriver* driver, const SlaveID& slaveId){}
virtual void executorLost(SchedulerDriver* driver, const ExecutorID& executorId, const SlaveID& slaveId, int status){}
virtual void error(SchedulerDriver* driver, const std::string& message){}
};

int main(int argc, char** argv)
{
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <master>" << endl;
    return -1; 
  }
  FrameworkInfo framework;
  framework.set_user(""); // Have Mesos fill in the current user.
  framework.set_name("Test Framework (C++)");
  MesosSchedulerDriver* driver;
  HelloScheduler scheduler;
  driver = new MesosSchedulerDriver(&scheduler, framework, argv[1]);
  int status = driver->run() == DRIVER_STOPPED ? 0 : 1;
  driver->stop();
  delete driver;
  return status;
}
