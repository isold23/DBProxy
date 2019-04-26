
killall -9 dbproxy

rm -rf ./dbproxy_log/*

nohup ./dbproxy -f ../conf/dbproxy.conf >/dev/null &
