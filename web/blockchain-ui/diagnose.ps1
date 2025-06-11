# PowerShell script to diagnose React app issues
Write-Host "=== React App Diagnostics ===" -ForegroundColor Cyan

# Check if ports are in use
Write-Host "`nChecking if ports are in use:" -ForegroundColor Yellow
$ports = @(3000, 3001, 3002, 3003, 8080, 8081, 8085, 8090)

foreach ($port in $ports) {
    $connection = Get-NetTCPConnection -LocalPort $port -ErrorAction SilentlyContinue
    if ($connection) {
        $process = Get-Process -Id $connection.OwningProcess -ErrorAction SilentlyContinue
        if ($process) {
            Write-Host "Port $port is in use by: $($process.Name) (PID: $($connection.OwningProcess))" -ForegroundColor Green
        } else {
            Write-Host "Port $port is in use by PID: $($connection.OwningProcess)" -ForegroundColor Green
        }
    } else {
        Write-Host "Port $port is NOT in use" -ForegroundColor Red
    }
}

# Check if the proxy middleware is installed
Write-Host "`nChecking if http-proxy-middleware is installed:" -ForegroundColor Yellow
$packageJsonPath = "package.json"
if (Test-Path $packageJsonPath) {
    $packageJson = Get-Content $packageJsonPath | ConvertFrom-Json
    if ($packageJson.dependencies.'http-proxy-middleware') {
        Write-Host "http-proxy-middleware is installed (version: $($packageJson.dependencies.'http-proxy-middleware'))" -ForegroundColor Green
    } else {
        Write-Host "http-proxy-middleware is NOT installed in dependencies!" -ForegroundColor Red
        Write-Host "Running: npm install http-proxy-middleware --save" -ForegroundColor Yellow
        npm install http-proxy-middleware --save
    }
} else {
    Write-Host "package.json not found!" -ForegroundColor Red
}

# Check if setupProxy.js exists and has valid content
Write-Host "`nChecking setupProxy.js:" -ForegroundColor Yellow
$setupProxyPath = "src\setupProxy.js"
if (Test-Path $setupProxyPath) {
    $setupProxyContent = Get-Content $setupProxyPath -Raw
    Write-Host "setupProxy.js exists with ${setupProxyContent.Length} characters" -ForegroundColor Green
    
    # Check for syntax errors in setupProxy.js
    try {
        node -c $setupProxyPath 2>$null
        if ($LASTEXITCODE -eq 0) {
            Write-Host "setupProxy.js has valid syntax" -ForegroundColor Green
        } else {
            Write-Host "setupProxy.js has syntax errors!" -ForegroundColor Red
            Write-Host "Fixing setupProxy.js..." -ForegroundColor Yellow
            
            # Create a valid setupProxy.js
            $newSetupProxyContent = @"
const { createProxyMiddleware } = require('http-proxy-middleware');

// Map UI ports to API ports
const portMapping = {
  3000: 8080,  // UI port 3000 -> API port 8080
  3001: 8090,  // UI port 3001 -> API port 8090
  3002: 8081,  // UI port 3002 -> API port 8081
  3003: 8085   // UI port 3003 -> API port 8085
};

module.exports = function(app) {
  // Get the current UI port from environment variable
  const currentPort = parseInt(process.env.PORT || '3000', 10);
  
  // Determine which API port to use based on the UI port
  const apiPort = portMapping[currentPort] || 8080; // Default to 8080 if no mapping
  
  console.log(`Setting up proxy from UI port ${currentPort} to API port ${apiPort}`);
  
  app.use(
    '/api',
    createProxyMiddleware({
      target: `http://localhost:${apiPort}`,
      changeOrigin: true,
      pathRewrite: {
        '^/api': '/api',
      },
      onProxyReq: function(proxyReq, req, res) {
        // Add CORS headers to proxy requests
        proxyReq.setHeader('Access-Control-Allow-Origin', '*');
        proxyReq.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
        proxyReq.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
        
        // Log the request for debugging
        console.log(`Proxying ${req.method} ${req.path} to http://localhost:${apiPort}${req.path}`);
      },
      onProxyRes: function(proxyRes, req, res) {
        // Add CORS headers to proxy responses
        proxyRes.headers['Access-Control-Allow-Origin'] = '*';
        proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS';
        proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization';
      },
      // Handle proxy errors
      onError: function(err, req, res) {
        console.error('Proxy error:', err);
        res.writeHead(500, {
          'Content-Type': 'text/plain',
          'Access-Control-Allow-Origin': '*',
          'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
          'Access-Control-Allow-Headers': 'Content-Type, Authorization'
        });
        res.end(`Proxy error: ${err.message}`);
      }
    })
  );
  
  // Add a special handler for OPTIONS requests
  app.use('/api/*', (req, res, next) => {
    if (req.method === 'OPTIONS') {
      res.header('Access-Control-Allow-Origin', '*');
      res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
      res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization');
      res.status(204).send();
      return;
    }
    next();
  });
};
"@
            
            Set-Content -Path $setupProxyPath -Value $newSetupProxyContent
            Write-Host "setupProxy.js has been fixed" -ForegroundColor Green
        }
    } catch {
        Write-Host "Error checking setupProxy.js syntax: $_" -ForegroundColor Red
    }
} else {
    Write-Host "setupProxy.js does not exist!" -ForegroundColor Red
    Write-Host "Creating setupProxy.js..." -ForegroundColor Yellow
    
    # Create a valid setupProxy.js
    $setupProxyContent = @"
const { createProxyMiddleware } = require('http-proxy-middleware');

// Map UI ports to API ports
const portMapping = {
  3000: 8080,  // UI port 3000 -> API port 8080
  3001: 8090,  // UI port 3001 -> API port 8090
  3002: 8081,  // UI port 3002 -> API port 8081
  3003: 8085   // UI port 3003 -> API port 8085
};

module.exports = function(app) {
  // Get the current UI port from environment variable
  const currentPort = parseInt(process.env.PORT || '3000', 10);
  
  // Determine which API port to use based on the UI port
  const apiPort = portMapping[currentPort] || 8080; // Default to 8080 if no mapping
  
  console.log(`Setting up proxy from UI port ${currentPort} to API port ${apiPort}`);
  
  app.use(
    '/api',
    createProxyMiddleware({
      target: `http://localhost:${apiPort}`,
      changeOrigin: true,
      pathRewrite: {
        '^/api': '/api',
      },
      onProxyReq: function(proxyReq, req, res) {
        // Add CORS headers to proxy requests
        proxyReq.setHeader('Access-Control-Allow-Origin', '*');
        proxyReq.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
        proxyReq.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
        
        // Log the request for debugging
        console.log(`Proxying ${req.method} ${req.path} to http://localhost:${apiPort}${req.path}`);
      },
      onProxyRes: function(proxyRes, req, res) {
        // Add CORS headers to proxy responses
        proxyRes.headers['Access-Control-Allow-Origin'] = '*';
        proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS');
        proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization';
      },
      // Handle proxy errors
      onError: function(err, req, res) {
        console.error('Proxy error:', err);
        res.writeHead(500, {
          'Content-Type': 'text/plain',
          'Access-Control-Allow-Origin': '*',
          'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
          'Access-Control-Allow-Headers': 'Content-Type, Authorization'
        });
        res.end(`Proxy error: ${err.message}`);
      }
    })
  );
  
  // Add a special handler for OPTIONS requests
  app.use('/api/*', (req, res, next) => {
    if (req.method === 'OPTIONS') {
      res.header('Access-Control-Allow-Origin', '*');
      res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
      res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization');
      res.status(204).send();
      return;
    }
    next();
  });
};
"@
    
    Set-Content -Path $setupProxyPath -Value $setupProxyContent
    Write-Host "setupProxy.js has been created" -ForegroundColor Green
}

# Check API availability
Write-Host "`nChecking if blockchain API endpoints are accessible:" -ForegroundColor Yellow
$apiPorts = @(8080, 8081, 8085, 8090)

foreach ($port in $apiPorts) {
    $url = "http://localhost:$port/api/wallet"
    Write-Host "Testing $url..." -NoNewline
    
    try {
        $response = Invoke-WebRequest -Uri $url -Method GET -TimeoutSec 2 -ErrorAction Stop
        Write-Host " SUCCESS (Status: $($response.StatusCode))" -ForegroundColor Green
    } catch {
        Write-Host " FAILED: $_" -ForegroundColor Red
    }
}

# Try to restart the app
Write-Host "`nWould you like to restart the React app? (y/n)" -ForegroundColor Cyan
$answer = Read-Host

if ($answer -eq "y") {
    Write-Host "`nRestarting React app on port 3000..." -ForegroundColor Yellow
    
    # Kill any existing processes on port 3000
    $processes = Get-NetTCPConnection -LocalPort 3000 -ErrorAction SilentlyContinue | 
                 Select-Object -ExpandProperty OwningProcess
    
    if ($processes) {
        foreach ($processId in $processes) {
            Write-Host "Killing process with PID: $processId" -ForegroundColor Yellow
            Stop-Process -Id $processId -Force -ErrorAction SilentlyContinue
        }
    }
    
    # Start the React app directly
    Write-Host "Starting React app..." -ForegroundColor Yellow
    Start-Process -FilePath "cmd.exe" -ArgumentList "/c set PORT=3000 && set REACT_APP_API_PORT=8080 && set BROWSER=none && npm start" -WorkingDirectory (Get-Location)
    
    Write-Host "React app should be starting. Check your browser at http://localhost:3000" -ForegroundColor Green
}

Write-Host "`nDiagnostics complete. If you're still having issues, try the following:" -ForegroundColor Cyan
Write-Host "1. Run 'npm install' to ensure all dependencies are installed" -ForegroundColor White
Write-Host "2. Check that your blockchain nodes are running properly" -ForegroundColor White
Write-Host "3. Try a different browser or clear your browser cache" -ForegroundColor White
Write-Host "4. Run 'npm run build' to create a production build and serve it" -ForegroundColor White 