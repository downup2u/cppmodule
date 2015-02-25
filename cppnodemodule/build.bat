echo "start generate proj......"
set curpath=%~dp0
cd %curpath%proj
node-gyp configure
echo "start build proj......"
node-gyp rebuild
echo "start copy......"
COPY /Y %curpath%/proj/build/Release/*.node %curpath%/output/win 
pause

