
killall -9 dbproxy_test
killall -9 dbproxy

rm -rf ./dbproxy_log/*

rm -rf ./dbproxy_test_log/*

nohup ./dbproxy -f ../conf/dbproxy.conf >/dev/null &
