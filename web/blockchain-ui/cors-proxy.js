const express = require('express');
const cors = require('cors');
const { createProxyMiddleware } = require('http-proxy-middleware');

const app = express();

// Enable CORS for all requests
app.use(cors({
  origin: '*',
  methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'Authorization']
}));

// Configure proxies for each blockchain node
const nodes = [
  { path: '/node1', target: 'http://localhost:8080' },
  { path: '/node2', target: 'http://localhost:8081' },
  { path: '/node3', target: 'http://localhost:8085' },
  { path: '/wallet', target: 'http://localhost:8090' }
];

// Create a proxy for each node
nodes.forEach(node => {
  app.use(node.path, createProxyMiddleware({
    target: node.target,
    changeOrigin: true,
    pathRewrite: {
      [`^${node.path}`]: '/api'
    },
    onProxyRes: function(proxyRes, req, res) {
      // Add CORS headers to the proxied response
      proxyRes.headers['Access-Control-Allow-Origin'] = '*';
      proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS';
      proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization';
    }
  }));
});

// Default route
app.get('/', (req, res) => {
  res.send(`
    <h1>Blockchain API CORS Proxy</h1>
    <p>Use the following endpoints to access the blockchain nodes:</p>
    <ul>
      <li><a href="/node1/statistics">/node1</a> - Node on port 8080</li>
      <li><a href="/node2/statistics">/node2</a> - Node on port 8081</li>
      <li><a href="/node3/statistics">/node3</a> - Node on port 8085</li>
      <li><a href="/wallet/statistics">/wallet</a> - Wallet node on port 8090</li>
    </ul>
  `);
});

// Start the server
const PORT = 8000;
app.listen(PORT, () => {
  console.log(`CORS Proxy running on http://localhost:${PORT}`);
}); 