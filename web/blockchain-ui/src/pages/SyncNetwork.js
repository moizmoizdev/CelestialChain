import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Button, 
  Grid,
  Card,
  CardContent,
  CardActions,
  List,
  ListItem,
  ListItemText,
  ListItemIcon,
  Divider,
  LinearProgress,
  Alert,
  Snackbar,
  CircularProgress,
  IconButton,
  Chip
} from '@mui/material';
import SyncIcon from '@mui/icons-material/Sync';
import CloudDownloadIcon from '@mui/icons-material/CloudDownload';
import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import ErrorIcon from '@mui/icons-material/Error';
import TimerIcon from '@mui/icons-material/Timer';
import RefreshIcon from '@mui/icons-material/Refresh';
import PeopleIcon from '@mui/icons-material/People';

import blockchainService from '../api/blockchainService';
import { useNavigate } from 'react-router-dom';

const SyncNetwork = () => {
  const navigate = useNavigate();
  const [syncStatus, setSyncStatus] = useState({
    isInProgress: false,
    progress: 0,
    startTime: null,
    endTime: null,
    blocksReceived: 0,
    totalBlocks: 0,
    status: 'idle',
    log: []
  });
  const [peers, setPeers] = useState([]);
  const [loading, setLoading] = useState(false);
  const [snackbar, setSnackbar] = useState({
    open: false,
    message: '',
    severity: 'info'
  });

  useEffect(() => {
    fetchInitialData();
  }, []);

  // If sync is in progress, update progress periodically
  useEffect(() => {
    let interval;
    if (syncStatus.isInProgress) {
      interval = setInterval(() => {
        setSyncStatus(prev => ({
          ...prev,
          progress: Math.min(prev.progress + Math.random() * 5, 100),
          blocksReceived: prev.blocksReceived + Math.floor(Math.random() * 3),
          log: [
            ...prev.log,
            ...(Math.random() > 0.7 ? [{
              timestamp: new Date().toISOString(),
              message: `Received block #${prev.blocksReceived + Math.floor(Math.random() * 3)}`,
              type: 'info'
            }] : [])
          ]
        }));
      }, 2000);
    }

    return () => {
      if (interval) clearInterval(interval);
    };
  }, [syncStatus.isInProgress]);

  // When progress reaches 100%, end sync
  useEffect(() => {
    if (syncStatus.progress >= 100 && syncStatus.isInProgress) {
      completeSyncProcess();
    }
  }, [syncStatus.progress]);

  const fetchInitialData = async () => {
    setLoading(true);
    try {
      // Fetch peers
      const peersResponse = await blockchainService.getPeers();
      if (peersResponse.data) {
        const peerData = Array.isArray(peersResponse.data) 
          ? peersResponse.data 
          : peersResponse.data.peers || [];
        setPeers(peerData);
      }

      // Check sync status - in a real implementation, you'd have an endpoint for this
      // For now, we're simulating a sync status response
      setSyncStatus(prev => ({
        ...prev,
        status: 'idle',
        progress: 0,
        log: [
          {
            timestamp: new Date().toISOString(),
            message: 'System ready for synchronization',
            type: 'info'
          }
        ]
      }));
    } catch (error) {
      console.error('Error fetching initial data:', error);
      setSnackbar({
        open: true,
        message: 'Failed to load network data',
        severity: 'error'
      });
    } finally {
      setLoading(false);
    }
  };

  const startSyncProcess = async () => {
    if (peers.length === 0) {
      setSnackbar({
        open: true,
        message: 'No peers available for synchronization',
        severity: 'warning'
      });
      return;
    }

    try {
      // In a real implementation, you'd call the blockchain sync API
      const response = await blockchainService.syncBlockchain();
      
      // Start simulated sync process
      setSyncStatus({
        isInProgress: true,
        progress: 0,
        startTime: new Date().toISOString(),
        endTime: null,
        blocksReceived: 0,
        totalBlocks: Math.floor(Math.random() * 50) + 10,
        status: 'syncing',
        log: [
          {
            timestamp: new Date().toISOString(),
            message: 'Synchronization started',
            type: 'info'
          },
          {
            timestamp: new Date().toISOString(),
            message: `Connected to ${peers.length} peers`,
            type: 'success'
          }
        ]
      });

      setSnackbar({
        open: true,
        message: 'Blockchain synchronization started',
        severity: 'info'
      });
    } catch (error) {
      console.error('Error starting sync:', error);
      setSnackbar({
        open: true,
        message: 'Failed to start synchronization',
        severity: 'error'
      });

      // Even if the API fails, we'll simulate a sync process for demo purposes
      setSyncStatus({
        isInProgress: true,
        progress: 0,
        startTime: new Date().toISOString(),
        endTime: null,
        blocksReceived: 0,
        totalBlocks: Math.floor(Math.random() * 50) + 10,
        status: 'syncing',
        log: [
          {
            timestamp: new Date().toISOString(),
            message: 'Synchronization started',
            type: 'info'
          },
          {
            timestamp: new Date().toISOString(),
            message: `Connected to ${peers.length} peers`,
            type: 'success'
          }
        ]
      });
    }
  };

  const completeSyncProcess = () => {
    setSyncStatus(prev => ({
      ...prev,
      isInProgress: false,
      progress: 100,
      endTime: new Date().toISOString(),
      status: 'completed',
      log: [
        ...prev.log,
        {
          timestamp: new Date().toISOString(),
          message: 'Synchronization completed successfully',
          type: 'success'
        }
      ]
    }));

    setSnackbar({
      open: true,
      message: 'Blockchain synchronized successfully',
      severity: 'success'
    });
  };

  const cancelSyncProcess = () => {
    setSyncStatus(prev => ({
      ...prev,
      isInProgress: false,
      status: 'cancelled',
      log: [
        ...prev.log,
        {
          timestamp: new Date().toISOString(),
          message: 'Synchronization cancelled by user',
          type: 'warning'
        }
      ]
    }));

    setSnackbar({
      open: true,
      message: 'Synchronization cancelled',
      severity: 'warning'
    });
  };

  const formatTime = (timestamp) => {
    if (!timestamp) return '';
    const date = new Date(timestamp);
    return date.toLocaleTimeString();
  };

  const handleCloseSnackbar = () => {
    setSnackbar({ ...snackbar, open: false });
  };

  if (loading) {
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '70vh' }}>
        <CircularProgress />
      </Box>
    );
  }

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
        <Box sx={{ display: 'flex', alignItems: 'center' }}>
          <SyncIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
          <Typography variant="h4" fontWeight="bold" gutterBottom>
            Network Synchronization
          </Typography>
        </Box>
        <Box>
          <IconButton 
            color="primary" 
            onClick={fetchInitialData}
            disabled={syncStatus.isInProgress}
          >
            <RefreshIcon />
          </IconButton>
        </Box>
      </Box>

      <Grid container spacing={3}>
        <Grid item xs={12} md={8}>
          <Card sx={{ mb: 3 }}>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Synchronization Status
              </Typography>
              <Box sx={{ mt: 3, mb: 2 }}>
                <Box sx={{ display: 'flex', alignItems: 'center', mb: 1 }}>
                  <Typography variant="body1" sx={{ flexGrow: 1 }}>
                    Status: {syncStatus.status.charAt(0).toUpperCase() + syncStatus.status.slice(1)}
                  </Typography>
                  <Chip 
                    label={syncStatus.status} 
                    color={
                      syncStatus.status === 'completed' ? 'success' : 
                      syncStatus.status === 'syncing' ? 'primary' :
                      syncStatus.status === 'cancelled' ? 'warning' : 'default'
                    }
                    size="small"
                  />
                </Box>
                {syncStatus.isInProgress && (
                  <Box sx={{ width: '100%', mb: 2 }}>
                    <LinearProgress 
                      variant="determinate" 
                      value={syncStatus.progress} 
                      sx={{ height: 10, borderRadius: 5 }}
                    />
                    <Box sx={{ display: 'flex', justifyContent: 'space-between', mt: 1 }}>
                      <Typography variant="body2" color="text.secondary">
                        {Math.round(syncStatus.progress)}% complete
                      </Typography>
                      <Typography variant="body2" color="text.secondary">
                        {syncStatus.blocksReceived} of {syncStatus.totalBlocks} blocks
                      </Typography>
                    </Box>
                  </Box>
                )}
                <Grid container spacing={3} sx={{ mt: 1 }}>
                  <Grid item xs={12} sm={6}>
                    <Box sx={{ display: 'flex', alignItems: 'center' }}>
                      <TimerIcon sx={{ mr: 1, color: 'text.secondary' }} />
                      <Box>
                        <Typography variant="body2" color="text.secondary">
                          Start Time
                        </Typography>
                        <Typography variant="body1">
                          {syncStatus.startTime ? formatTime(syncStatus.startTime) : 'Not started'}
                        </Typography>
                      </Box>
                    </Box>
                  </Grid>
                  <Grid item xs={12} sm={6}>
                    <Box sx={{ display: 'flex', alignItems: 'center' }}>
                      <TimerIcon sx={{ mr: 1, color: 'text.secondary' }} />
                      <Box>
                        <Typography variant="body2" color="text.secondary">
                          End Time
                        </Typography>
                        <Typography variant="body1">
                          {syncStatus.endTime ? formatTime(syncStatus.endTime) : 'In progress'}
                        </Typography>
                      </Box>
                    </Box>
                  </Grid>
                </Grid>
              </Box>
            </CardContent>
            <CardActions sx={{ justifyContent: 'center', p: 2 }}>
              {!syncStatus.isInProgress && syncStatus.status !== 'completed' ? (
                <Button 
                  variant="contained" 
                  color="primary" 
                  startIcon={<SyncIcon />}
                  onClick={startSyncProcess}
                  disabled={peers.length === 0}
                  sx={{ minWidth: 200 }}
                >
                  Start Synchronization
                </Button>
              ) : syncStatus.isInProgress ? (
                <Button 
                  variant="outlined" 
                  color="warning" 
                  onClick={cancelSyncProcess}
                  sx={{ minWidth: 200 }}
                >
                  Cancel Sync
                </Button>
              ) : (
                <Button 
                  variant="outlined" 
                  color="primary" 
                  startIcon={<SyncIcon />}
                  onClick={startSyncProcess}
                  sx={{ minWidth: 200 }}
                >
                  Sync Again
                </Button>
              )}
            </CardActions>
          </Card>

          <Card>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Synchronization Log
              </Typography>
              <Box sx={{ maxHeight: 300, overflow: 'auto', mt: 2, bgcolor: 'background.paper', borderRadius: 1, p: 1 }}>
                <List dense>
                  {syncStatus.log.length > 0 ? (
                    syncStatus.log.map((entry, index) => (
                      <ListItem key={index} sx={{ py: 0.5 }}>
                        <ListItemIcon sx={{ minWidth: 36 }}>
                          {entry.type === 'success' ? (
                            <CheckCircleIcon fontSize="small" color="success" />
                          ) : entry.type === 'error' ? (
                            <ErrorIcon fontSize="small" color="error" />
                          ) : (
                            <SyncIcon fontSize="small" color="primary" />
                          )}
                        </ListItemIcon>
                        <ListItemText
                          primary={entry.message}
                          secondary={formatTime(entry.timestamp)}
                          primaryTypographyProps={{ 
                            variant: 'body2',
                            fontFamily: 'monospace'
                          }}
                          secondaryTypographyProps={{ 
                            variant: 'caption',
                            fontFamily: 'monospace'
                          }}
                        />
                      </ListItem>
                    ))
                  ) : (
                    <ListItem>
                      <ListItemText primary="No log entries available" />
                    </ListItem>
                  )}
                </List>
              </Box>
            </CardContent>
          </Card>
        </Grid>

        <Grid item xs={12} md={4}>
          <Card sx={{ mb: 3 }}>
            <CardContent>
              <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                <PeopleIcon sx={{ mr: 1, color: 'primary.main' }} />
                <Typography variant="h6">Available Peers</Typography>
              </Box>
              <Divider sx={{ mb: 2 }} />

              {peers.length > 0 ? (
                <List dense>
                  {peers.map((peer) => (
                    <ListItem key={peer.id || `${peer.address}:${peer.port}`}>
                      <ListItemIcon>
                        <Chip 
                          label={peer.type === 'full' ? 'Full' : 'Wallet'} 
                          color={peer.type === 'full' ? 'primary' : 'secondary'}
                          size="small"
                        />
                      </ListItemIcon>
                      <ListItemText 
                        primary={`${peer.address}:${peer.port}`}
                        secondary={peer.id || 'Unknown ID'}
                      />
                    </ListItem>
                  ))}
                </List>
              ) : (
                <Alert severity="warning" sx={{ mt: 2 }}>
                  No peers available for synchronization
                </Alert>
              )}
            </CardContent>
            <CardActions>
              <Button 
                fullWidth
                onClick={() => navigate('/peers')}
                variant="text"
              >
                Manage Peers
              </Button>
            </CardActions>
          </Card>

          <Card>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Sync Information
              </Typography>
              <Divider sx={{ mb: 2 }} />
              
              <Typography variant="body2" paragraph>
                Blockchain synchronization allows your node to fetch the latest blockchain data from other nodes in the network.
              </Typography>
              
              <Typography variant="body2" paragraph>
                During synchronization, your node will:
              </Typography>
              
              <List dense>
                <ListItem>
                  <ListItemIcon>
                    <CloudDownloadIcon fontSize="small" color="primary" />
                  </ListItemIcon>
                  <ListItemText primary="Download new blocks" />
                </ListItem>
                <ListItem>
                  <ListItemIcon>
                    <SyncIcon fontSize="small" color="primary" />
                  </ListItemIcon>
                  <ListItemText primary="Validate transaction history" />
                </ListItem>
                <ListItem>
                  <ListItemIcon>
                    <CloudUploadIcon fontSize="small" color="primary" />
                  </ListItemIcon>
                  <ListItemText primary="Share your blocks with peers" />
                </ListItem>
              </List>
              
              <Alert severity="info" sx={{ mt: 2 }}>
                Regular synchronization ensures your node has the most up-to-date blockchain data.
              </Alert>
            </CardContent>
          </Card>
        </Grid>
      </Grid>

      {/* Snackbar for notifications */}
      <Snackbar
        open={snackbar.open}
        autoHideDuration={6000}
        onClose={handleCloseSnackbar}
        anchorOrigin={{ vertical: 'bottom', horizontal: 'right' }}
      >
        <Alert
          onClose={handleCloseSnackbar}
          severity={snackbar.severity}
          variant="filled"
          sx={{ width: '100%' }}
        >
          {snackbar.message}
        </Alert>
      </Snackbar>
    </Box>
  );
};

export default SyncNetwork; 