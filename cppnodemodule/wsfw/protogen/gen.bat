cd commProtosrc
protoc.exe  -I=../ --cpp_out=../../src/proto/custommsg  ../nosqldb.proto
protoc.exe  -I=../ --cpp_out=../../src/proto/custommsg  ../xkcrm.proto
protoc.exe  -I=../ --cpp_out=../../src/proto/  ../packmsg.proto
protoc.exe  -I=../ --cpp_out=../../src/wssrv/couchbase  ../couchbasesrv.proto
pause