const axios = require('axios');

// Test various API endpoints
async function testEndpoints() {
  const apiPort = 8080;
  const endpoints = [
    '/api/statistics',
    '/api/explorer/latest-blocks?limit=5',
    '/api/explorer/latest-transactions?limit=5',
    '/api/wallet',
    '/api/peers',
    '/api/mempool'
  ];

  console.log('Testing direct API connections...');

  for (const endpoint of endpoints) {
    try {
      console.log(`Testing ${endpoint}...`);
      const response = await axios.get(`http://localhost:${apiPort}${endpoint}`, {
        headers: {
          'Content-Type': 'application/json'
        }
      });
      
      console.log(`[SUCCESS] ${endpoint}: Status ${response.status}`);
      console.log(JSON.stringify(response.data, null, 2).substring(0, 100) + '...');
    } catch (error) {
      console.error(`[ERROR] ${endpoint}: ${error.message}`);
      if (error.response) {
        console.error(`  Status: ${error.response.status}`);
        console.error(`  Data: ${JSON.stringify(error.response.data)}`);
      }
    }
    console.log('---');
  }
}

testEndpoints().catch(err => {
  console.error('Test failed:', err);
}); 