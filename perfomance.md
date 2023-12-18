
## Данные

### Без кэша

```
kopistas@vm-server:~/ais-course$ sh test_no_cache.sh
Running 10s test @ http://localhost:8082/
  50 threads and 200 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     9.80ms    5.47ms  48.89ms   71.67%
    Req/Sec   409.36    130.59   820.00     69.06%
  Latency Distribution
     50%    8.62ms
     75%   12.66ms
     90%   17.25ms
     99%   26.93ms
  16527 requests in 10.10s, 4.81MB read
  Socket errors: connect 0, read 144833, write 0, timeout 0
  Non-2xx or 3xx responses: 8295
Requests/sec:   1636.53
Transfer/sec:    487.60KB
```

### С кэшем в первый раз
```
kopistas@vm-server:~/ais-course$ sh test_cache.sh
Running 10s test @ http://localhost:8082/
  50 threads and 200 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     8.16ms    6.80ms  49.57ms   71.60%
    Req/Sec   491.84    351.77     1.64k    79.46%
  Latency Distribution
     50%    6.18ms
     75%   12.13ms
     90%   17.91ms
     99%   28.55ms
  19839 requests in 10.10s, 5.80MB read
  Socket errors: connect 0, read 182055, write 0, timeout 0
  Non-2xx or 3xx responses: 9250
Requests/sec:   1964.81
Transfer/sec:    588.10KB
```

### C кэшем через пару-тройку раз
```
kopistas@vm-server:~/ais-course$ sh test_cache.sh
Running 10s test @ http://localhost:8082/
  50 threads and 200 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     5.68ms    4.60ms  44.45ms   76.18%
    Req/Sec   705.85    253.56     1.18k    61.39%
  Latency Distribution
     50%    4.58ms
     75%    7.81ms
     90%   11.69ms
     99%   21.25ms
  28469 requests in 10.10s, 9.10MB read
  Socket errors: connect 0, read 222997, write 0, timeout 0
  Non-2xx or 3xx responses: 3173
Requests/sec:   2818.73
Transfer/sec:      0.90MB
```

## Вывод

Кэш ускоряет производтельность, но делает это не обязательно сразу - сначала необходимо время для того чтобы значимое количество данных успело закэшироваться. 