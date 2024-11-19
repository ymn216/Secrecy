# Secrecy Benchmark

### 1. queries：

q1:comorbidity（没有join，但IN很慢）

```sql
SELECT diag, COUNT(*) cnt
FROM diagnoses
WHERE patient_id IN cdiff_cohort
GROUP BY diag
ORDER BY cnt
LIMIT 10;
```

q2:rec c.diff（自连接)

```sql
WITH rcd AS (
	SELECT pid, time, row_no() OVER
		(PARTITION BY pid ORDER BY time)
	FROM diagnosis
	WHERE diag=cdiff)
SELECT DISTINCT pid
FROM rcd r1 JOIN rcd r2 ON r1.pid = r2.pid
WHERE r2.time - r1.time >= 15 DAYS
	AND r2.time - r1.time <= 56 DAYS
	AND r2.row_no = r1.row_no + 1;
```

q3:Aspirin（join）

```sql
SELECT COUNT(DISTINCT pid)
FROM diagnosis d JOIN medication m ON d.pid = m.pid
WHERE d.diag = hd AND m.med = aspirin
	AND d.time <= m.time;
```

tpch_q4（join）

```sql
SELECT o_orderpriority, count(*) as order_count
FROM orders
WHERE o_orderdate >= 'date1' AND
      o_orderdate < 'date2'
      AND EXISTS (
        SELECT *
        FROM lineitem
        WHERE l_orderkey = o_orderkey
              AND l_commitdate < l_receiptdate
      )
GROUP BY o_orderpriority
ORDER BY o_orderpriority
```

tpch_q6（无join）

```sql
SELECT sum(l_extendedprice * l_discount) as revenue
FROM lineitem
WHERE l_shipdate >= 'date1' AND
   l_shipdate < 'date2'
   AND l_discount <= 'discount' - 0.01
   AND l_discount >= 'discount' + 0.01
   AND l_quantity < 'quantity'
```

tpch_q13（join）

```sql
SELECT c_count, count(*) as custdist
FROM (
    SELECT c_custkey, count(o_orderkey)
    FROM customer left outer join orders ON
      c_custkey = o_custkey
      AND o_comment = 'word'
    GROUP BY c_custkey) as c_orders(c_custkey, c_count)
GROUP BY c_count
ORDER BY custdist DESC, c_count DESC
```

### 2.Benchmark

ROWS：$2^{10}$,$2^{11}$,$2^{12}$,$2^{13}$,$2^{14}$,$2^{15}$

#### 运行测试：

```cmd
./src/benchmarking/q1.sh
./src/benchmarking/q2.sh
./src/benchmarking/q3.sh
./src/benchmarking/tpch_q4.sh
./src/benchmarking/tpch_q6.sh
./src/benchmarking/tpch_q13.sh
```

#### 运行结果：

**Secrecy/benchmark_result** 文件夹中



