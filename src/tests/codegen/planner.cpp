#include "codegen-utils.h"
#include <chrono>

// 记录时间的辅助函数
auto getCurrentTime() -> std::chrono::high_resolution_clock::time_point {
    return std::chrono::high_resolution_clock::now();
}

void printElapsedTime(const std::string& message, const std::chrono::high_resolution_clock::time_point& start) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        auto end = getCurrentTime();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << message << " 耗时: " << elapsed.count() << " 毫秒" << std::endl;
    }
}

// Generates the optimal plan for the given query
void executeQuery(int argc, char *argv[]){
    std::shared_ptr<Database> db;
    std::shared_ptr<Query> query;
    // 记录开始时间
    auto start = getCurrentTime();

    // Create (empty) database and parse query
    leaderLogging("[INFO] Creating DB and parsing query...\n");
    createDBandQuery(argc, argv, "Example_DB", db, query);
    leaderLogging("[INFO] Done.\n");
    printElapsedTime("创建数据库和解析查询", start);
    
    // Print DB schema to stdout
    start = getCurrentTime();
    db->printSchema();
    printElapsedTime("打印数据库模式", start);

    // Populate database relations with random data
    start = getCurrentTime();
    std::vector<DataTable> r_data;
    populateRandDB(db, r_data);
    printElapsedTime("填充数据库关系数据", start);

    // Parse query and generate plan
    //false:no physical plan 不生成物理计划，只执行预处理和生成最优计划的部分
    //query->generatePlan(false);
    start = getCurrentTime();
    query->generatePlan();
    assert(!query->plans.empty());
    printElapsedTime("解析查询并生成计划", start);

    // Exchange seeds
    start = getCurrentTime();
    exchange_rsz_seeds();
    printElapsedTime("交换种子", start);

    // Call execution on the root operator
    start = getCurrentTime();
    leaderLogging("[INFO] Executing the plan\n");
    query->root->execute();
    leaderLogging("[INFO] Done.\n");
    printElapsedTime("执行查询", start);

    // Free random input data
    auto schema = db->getRelations();
    int rel_no = schema.size();
    for (int i=0; i<rel_no; i++) {
        free(r_data[i]);
        schema[i]->freeRelation();
    }
}


int main(int argc, char *argv[]) {
    // Initialize MPI communication
    init(argc, argv);
    init_sharing();
    // Compile and execute the query
    executeQuery(argc, argv);
    // Shut down
    MPI_Finalize();
    return 0;
}