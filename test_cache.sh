wrk -d 10 -t 50 -c 200 --latency -s ./get_cache.lua http://localhost:8082/