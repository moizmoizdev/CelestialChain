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
        'Accept': 'application/json'
      }
    };

    console.log(`Making direct HTTP request to: ${host}:${port}${path}`);

    const req = http.request(options, (res) => {
      console.log(`STATUS: ${res.statusCode}`);
      console.log(`HEADERS: ${JSON.stringify(res.headers, null, 2)}`);
      
      let data = Buffer.from('');
      
      res.on('data', (chunk) => {
        data = Buffer.concat([data, chunk]);
      });
      
      res.on('end', () => {
        console.log(`BODY length: ${data.length} bytes`);
        if (data.length > 0) {
          const dataStr = data.toString('utf8');
          console.log(`Raw response: ${dataStr}`);
          
          try {
            const parsedData = JSON.parse(dataStr);
            resolve({
              statusCode: res.statusCode,
              headers: res.headers,
              data: parsedData
            });
          } catch (e) {
            console.log(`Error parsing JSON: ${e.message}`);
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

// Test all the endpoints
async function testAllEndpoints() {
  const endpoints = [
    '/api/statistics',
    '/api/explorer/latest-blocks?limit=5',
    '/api/explorer/latest-transactions?limit=5',
    '/api/wallet',
    '/api/peers',
    '/api/mempool'
  ];
  
  for (const endpoint of endpoints) {
    console.log(`\n=== Testing endpoint: ${endpoint} ===`);
    try {
      const response = await directHttpRequest('localhost', 8080, endpoint);
      console.log('Response status:', response.statusCode);
      if (response.data) {
        console.log('Parsed data:', JSON.stringify(response.data, null, 2).substring(0, 500) + '...');
      }
    } catch (error) {
      console.error('Error:', error.message);
    }
  }
}

// Run the test
testAllEndpoints(); 