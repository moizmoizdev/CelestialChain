const { createProxyMiddleware } = require('http-proxy-middleware'); 
 
module.exports = function(app) { 
  app.use( 
    '/api', 
    createProxyMiddleware({ 
      target: 'http://localhost:8081', 
      changeOrigin: true, 
      pathRewrite: { 
        '/api': '/api', 
      }, 
      onProxyRes: function(proxyRes, req, res) { 
        proxyRes.headers['Access-Control-Allow-Origin'] = '*'; 
        proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'; 
        proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'; 
      } 
    }) 
  ); 
}; 
