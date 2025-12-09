#include <gtest/gtest.h>
#include "dag.hpp"
#include "job.hpp"
#include "semaphore.hpp"

TEST(DAGTest, EmptyDAG) {
    DAG dag;
    EXPECT_TRUE(dag.isValid());
}

TEST(DAGTest, SingleJob) {
    auto job = std::make_shared<Job>("job1", "echo 'test'");
    DAG dag;
    dag.addJob(job);
    dag.buildDependencies();
    
    EXPECT_TRUE(dag.isValid());
    EXPECT_EQ(dag.getStartJobs().size(), 1);
}

TEST(DAGTest, ValidChain) {
    auto job1 = std::make_shared<Job>("job1", "echo '1'");
    auto job2 = std::make_shared<Job>("job2", "echo '2'");
    auto job3 = std::make_shared<Job>("job3", "echo '3'");
    
    job2->addDependency("job1");
    job3->addDependency("job2");
    
    DAG dag;
    dag.addJob(job1);
    dag.addJob(job2);
    dag.addJob(job3);
    dag.buildDependencies();
    
    EXPECT_TRUE(dag.isValid());
    EXPECT_FALSE(dag.hasCycles());
    EXPECT_TRUE(dag.hasSingleConnectedComponent());
}

TEST(DAGTest, CycleDetection) {
    auto job1 = std::make_shared<Job>("job1", "echo '1'");
    auto job2 = std::make_shared<Job>("job2", "echo '2'");
    auto job3 = std::make_shared<Job>("job3", "echo '3'");
    
    job1->addDependency("job3");
    job2->addDependency("job1");
    job3->addDependency("job2");
    
    DAG dag;
    dag.addJob(job1);
    dag.addJob(job2);
    dag.addJob(job3);
    dag.buildDependencies();
    
    EXPECT_TRUE(dag.hasCycles());
    EXPECT_FALSE(dag.isValid());
}

TEST(DAGTest, SemaphoreIntegration) {
    auto sem = std::make_shared<Semaphore>("test_sem", 2);
    
    auto job1 = std::make_shared<Job>("job1", "sleep 0.1");
    auto job2 = std::make_shared<Job>("job2", "sleep 0.1");
    auto job3 = std::make_shared<Job>("job3", "sleep 0.1");
    
    job1->setSemaphore(sem);
    job2->setSemaphore(sem);
    job3->setSemaphore(sem);
    
    EXPECT_TRUE(job1->canRun());
    EXPECT_TRUE(job2->canRun());
    EXPECT_FALSE(job3->canRun()); // Семафор позволяет только 2 задачи
}