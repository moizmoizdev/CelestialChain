const http = require('http');

// Make a direct HTTP GET request without using axios or any proxy
function directHttpRequest(host, port, path) {
  return new Promise((resolve, reject) => {
    const options = {
      hostname: host,
      port: port,
      path: path,
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
        'Accept': 'application/json'
      }
    };

    console.log(`Making direct HTTP request to: ${host}:${port}${path}`);

    const req = http.request(options, (res) => {
      console.log(`STATUS: ${res.statusCode}`);
      console.log(`HEADERS: ${JSON.stringify(res.headers)}`);
      
      let data = '';
      
      res.on('data', (chunk) => {
        data += chunk;
      });
      
      res.on('end', () => {
        console.log(`BODY length: ${data.length} bytes`);
        if (data.length > 0) {
          try {
            const parsedData = JSON.parse(data);
            resolve({
              statusCode: res.statusCode,
              headers: res.headers,
              data: parsedData
            });
          } catch (e) {
            console.log(`Raw response data: ${data.substring(0, 100)}...`);
            reject(new Error(`Invalid JSON response: ${e.message}`));
          }
        } else {
          console.log('Empty response body');
          resolve({
            statusCode: res.statusCode,
            headers: res.headers,
            data: null
          });
        }
      });
    });

    req.on('error', (e) => {
      console.error(`Request error: ${e.message}`);
      reject(e);
    });

    // End the request
    req.end();
  });
}

// Test the statistics endpoint
async function testStatistics() {
  try {
    const response = await directHttpRequest('localhost', 8080, '/api/statistics');
    console.log('Statistics endpoint response:');
    console.log(JSON.stringify(response.data, null, 2));
  } catch (error) {
    console.error('Error testing statistics endpoint:', error.message);
  }
}

// Run the test
testStatistics(); 