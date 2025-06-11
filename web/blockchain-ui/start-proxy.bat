@echo off
echo Starting Blockchain UI CORS Proxy...

cd %~dp0
npm install express cors http-proxy-middleware --save
node cors-proxy.js

echo CORS Proxy stopped. 