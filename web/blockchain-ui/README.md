# CelestialChain Blockchain UI

A modern user interface for interacting with the CelestialChain blockchain network. This application allows you to connect to and manage multiple blockchain nodes and wallets from a single interface.

## Features

- Dashboard with blockchain network statistics
- Blockchain explorer for viewing blocks and transactions
- Mempool monitoring
- Mining interface
- Wallet management
- Peer network management
- Multi-node support with dynamic node switching
- Blockchain synchronization tools
- Network settings configuration

## Getting Started

### Prerequisites

- Node.js 16+ and npm
- Running CelestialChain blockchain nodes (full nodes and/or wallet nodes)

### Installation

1. Clone the repository
2. Install dependencies:

```
cd web/blockchain-ui
npm install
```

## Running the Application

### Single Instance

To run a single instance of the UI connecting to a default blockchain node (localhost:8080):

```
npm start
```

This will start the application at http://localhost:3000.

### Multiple Instances

The application supports connecting to multiple blockchain nodes simultaneously by running multiple UI instances. This is useful for:

- Monitoring different nodes in your network
- Using both full nodes and wallet nodes
- Testing peer-to-peer communication

#### Using PowerShell Script (Windows)

1. Run the PowerShell script to launch multiple UI instances:

```
.\start-multi-ui.ps1
```

This will:
- Open separate terminal windows for each UI instance
- Configure each instance to connect to a different blockchain node API
- Make each UI instance available at a different port

#### Using Node.js Script

1. Run the Node.js script with an optional port parameter:

```
node run-instances.js [port]
```

Without specifying a port, it will run the first configured instance.

For example:
- `node run-instances.js 3000` - Starts a UI instance on port 3000 connecting to the API on port 8080
- `node run-instances.js 3001` - Starts a UI instance on port 3001 connecting to the API on port 8090

#### Manually Configuring Instances

You can also manually configure each instance by:

1. Creating a `.env.{port}` file with the appropriate configuration
2. Updating the `setupProxy.js` file to point to the desired API port
3. Starting the application with specific environment variables:

```
PORT=3001 REACT_APP_API_PORT=8090 npm start
```

## Node Connectivity

The UI allows you to:
- Switch between different nodes within the same UI instance using the node selector in the header
- Add, edit, and remove node connections
- Test connections to verify node availability

By default, the following nodes are configured:
- Local Node 1 (http://localhost:8080/api)
- Local Node 2 (http://localhost:8081/api)
- Local Node 3 (http://localhost:8085/api)
- Local Wallet (http://localhost:8090/api)

## Development

This project was bootstrapped with [Create React App](https://github.com/facebook/create-react-app).

### Available Scripts

- `npm start` - Runs the app in development mode
- `npm test` - Launches the test runner
- `npm run build` - Builds the app for production

## Learn More

You can learn more in the [Create React App documentation](https://facebook.github.io/create-react-app/docs/getting-started).

To learn React, check out the [React documentation](https://reactjs.org/).

### Code Splitting

This section has moved here: [https://facebook.github.io/create-react-app/docs/code-splitting](https://facebook.github.io/create-react-app/docs/code-splitting)

### Analyzing the Bundle Size

This section has moved here: [https://facebook.github.io/create-react-app/docs/analyzing-the-bundle-size](https://facebook.github.io/create-react-app/docs/analyzing-the-bundle-size)

### Making a Progressive Web App

This section has moved here: [https://facebook.github.io/create-react-app/docs/making-a-progressive-web-app](https://facebook.github.io/create-react-app/docs/making-a-progressive-web-app)

### Advanced Configuration

This section has moved here: [https://facebook.github.io/create-react-app/docs/advanced-configuration](https://facebook.github.io/create-react-app/docs/advanced-configuration)

### Deployment

This section has moved here: [https://facebook.github.io/create-react-app/docs/deployment](https://facebook.github.io/create-react-app/docs/deployment)

### `npm run build` fails to minify

This section has moved here: [https://facebook.github.io/create-react-app/docs/troubleshooting#npm-run-build-fails-to-minify](https://facebook.github.io/create-react-app/docs/troubleshooting#npm-run-build-fails-to-minify)
