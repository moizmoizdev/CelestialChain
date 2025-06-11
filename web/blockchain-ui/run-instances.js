const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');

// Configuration for multiple instances
const instances = [
  { port: 3000, name: 'Blockchain UI - Full Node', apiPort: 8080 },
  { port: 3001, name: 'Blockchain UI - Wallet Node', apiPort: 8090 },
  { port: 3002, name: 'Blockchain UI - Full Node 2', apiPort: 8081 },
];

// Create a .env file for each instance
const createEnvFile = (port, apiPort) => {
  const envContent = `
PORT=${port}
REACT_APP_API_PORT=${apiPort}
REACT_APP_NODE_NAME=${instances.find(i => i.port === port).name}
`;
  fs.writeFileSync(path.join(__dirname, `.env.${port}`), envContent.trim());
  console.log(`Created .env.${port} file with PORT=${port} and API_PORT=${apiPort}`);
};

// Create proxy configuration file for each instance
const createProxyConfig = (port, apiPort) => {
  const configContent = `
const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function(app) {
  app.use(
    '/api',
    createProxyMiddleware({
      target: 'http://localhost:${apiPort}',
      changeOrigin: true,
      pathRewrite: {
        '^/api': '/api', // no rewrite needed, but it's explicit
      },
    })
  );
};
`;
  const configDir = path.join(__dirname, 'src', 'setupProxy.js');
  fs.writeFileSync(configDir, configContent.trim());
  console.log(`Updated proxy configuration for port ${port} -> API port ${apiPort}`);
};

// Start a React instance with specific port and name
const startInstance = (port, name, apiPort) => {
  console.log(`Starting instance "${name}" on port ${port}, connecting to API on port ${apiPort}`);
  
  // Create the appropriate config files
  createEnvFile(port, apiPort);
  createProxyConfig(port, apiPort);
  
  // Build the environment variables object
  const env = {
    ...process.env,
    PORT: port.toString(),
    REACT_APP_API_PORT: apiPort.toString(),
    REACT_APP_NODE_NAME: name,
  };

  // Start the React application with the custom environment
  const reactProcess = spawn('npm', ['start'], {
    env,
    shell: true,
    stdio: 'inherit',
    env: {
      ...process.env,
      BROWSER: 'none', // Don't open browser window automatically
    }
  });

  reactProcess.on('error', (error) => {
    console.error(`Error starting ${name}:`, error);
  });

  return reactProcess;
};

// Check if a specific instance is requested
const requestedPort = process.argv[2] ? parseInt(process.argv[2]) : null;

if (requestedPort) {
  const instance = instances.find(i => i.port === requestedPort);
  if (instance) {
    startInstance(instance.port, instance.name, instance.apiPort);
  } else {
    console.error(`No instance configuration found for port ${requestedPort}`);
    console.log('Available ports:', instances.map(i => i.port).join(', '));
  }
} else {
  // Start the first instance by default
  const instance = instances[0];
  startInstance(instance.port, instance.name, instance.apiPort);
  
  console.log('\nTo start other instances, run:');
  instances.slice(1).forEach(instance => {
    console.log(`node run-instances.js ${instance.port}`);
  });
}

console.log('\nKeep this terminal window open to keep the server running.');
console.log('Press Ctrl+C to stop the server.\n'); 