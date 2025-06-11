import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Chip, 
  Tooltip, 
  CircularProgress
} from '@mui/material';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import ErrorIcon from '@mui/icons-material/Error';
import HelpIcon from '@mui/icons-material/Help';
import blockchainService from '../api/blockchainService';

const NodeStatus = () => {
  const [status, setStatus] = useState({
    isConnected: false,
    isChecking: true,
    nodeType: null,
    apiPort: null,
    message: 'Checking connection...'
  });

  const nodeManager = blockchainService.getNodeManager();

  useEffect(() => {
    checkNodeConnection();

    // Set up a listener for node changes
    const handleNodeChange = () => {
      checkNodeConnection();
    };

    nodeManager.addListener(handleNodeChange);

    // Set up interval to check connection periodically
    const interval = setInterval(() => {
      checkNodeConnection();
    }, 30000); // Check every 30 seconds

    return () => {
      clearInterval(interval);
      nodeManager.removeListener(handleNodeChange);
    };
  }, []);

  const checkNodeConnection = async () => {
    setStatus(prev => ({ ...prev, isChecking: true }));
    
    try {
      const response = await blockchainService.getStatistics();
      
      if (response && response.data) {
        // Extract node type and port if available
        const nodeType = response.data.nodeType || 'Unknown';
        const apiPort = response.data.apiPort || nodeManager.getActiveNode().url.match(/\d+/g)?.[0] || 'Unknown';
        
        setStatus({
          isConnected: true,
          isChecking: false,
          nodeType,
          apiPort,
          message: `Connected to ${nodeType} node`
        });
      } else {
        setStatus({
          isConnected: false,
          isChecking: false,
          nodeType: null,
          apiPort: null,
          message: 'Node returned invalid data'
        });
      }
    } catch (error) {
      console.error('Node connection check failed:', error);
      setStatus({
        isConnected: false,
        isChecking: false,
        nodeType: null,
        apiPort: null,
        message: `Connection failed: ${error.message}`
      });
    }
  };

  return (
    <Tooltip title={status.message}>
      <Box sx={{ display: 'flex', alignItems: 'center' }}>
        {status.isChecking ? (
          <CircularProgress size={16} color="inherit" sx={{ mr: 1 }} />
        ) : status.isConnected ? (
          <CheckCircleIcon color="success" fontSize="small" sx={{ mr: 1 }} />
        ) : (
          <ErrorIcon color="error" fontSize="small" sx={{ mr: 1 }} />
        )}
        
        <Chip
          variant="outlined"
          size="small"
          color={status.isConnected ? "success" : "error"}
          label={
            <Typography variant="caption">
              {status.isConnected 
                ? `${status.nodeType} (Port: ${status.apiPort})`
                : "Disconnected"
              }
            </Typography>
          }
          onClick={checkNodeConnection}
        />
      </Box>
    </Tooltip>
  );
};

export default NodeStatus; 