import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Button, 
  TextField, 
  Grid,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  IconButton,
  Chip,
  CircularProgress,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Alert,
  Snackbar
} from '@mui/material';
import RefreshIcon from '@mui/icons-material/Refresh';
import DeleteIcon from '@mui/icons-material/Delete';
import AddIcon from '@mui/icons-material/Add';
import LinkIcon from '@mui/icons-material/Link';
import PeopleIcon from '@mui/icons-material/People';

import blockchainService from '../api/blockchainService';

const Peers = () => {
  const [peers, setPeers] = useState([]);
  const [loading, setLoading] = useState(false);
  const [connectDialogOpen, setConnectDialogOpen] = useState(false);
  const [peerAddress, setPeerAddress] = useState('');
  const [peerPort, setPeerPort] = useState('');
  const [snackbar, setSnackbar] = useState({
    open: false,
    message: '',
    severity: 'info'
  });

  useEffect(() => {
    fetchPeers();
  }, []);

  const fetchPeers = async () => {
    setLoading(true);
    try {
      const response = await blockchainService.getPeers();
      if (response.data) {
        const peerData = Array.isArray(response.data) 
          ? response.data 
          : response.data.peers || [];
        setPeers(peerData);
      }
    } catch (error) {
      console.error('Error fetching peers:', error);
      setSnackbar({
        open: true,
        message: 'Failed to load peers',
        severity: 'error'
      });
    } finally {
      setLoading(false);
    }
  };

  const handleConnectPeer = async () => {
    if (!peerAddress || !peerPort) {
      setSnackbar({
        open: true,
        message: 'Please enter both address and port',
        severity: 'warning'
      });
      return;
    }

    setLoading(true);
    try {
      const response = await blockchainService.connectToPeer(peerAddress, parseInt(peerPort, 10));
      if (response.data && response.data.success) {
        setSnackbar({
          open: true,
          message: 'Successfully connected to peer',
          severity: 'success'
        });
        setConnectDialogOpen(false);
        setPeerAddress('');
        setPeerPort('');
        fetchPeers();
      } else {
        setSnackbar({
          open: true,
          message: 'Failed to connect to peer',
          severity: 'error'
        });
      }
    } catch (error) {
      console.error('Error connecting to peer:', error);
      setSnackbar({
        open: true,
        message: 'Error connecting to peer',
        severity: 'error'
      });
    } finally {
      setLoading(false);
    }
  };

  // In a real app you'd have a disconnect function
  const handleDisconnectPeer = (peerId) => {
    // This is a mock implementation since we can't actually disconnect peers through API
    setPeers(peers.filter(peer => peer.id !== peerId));
    setSnackbar({
      open: true,
      message: 'Peer disconnected',
      severity: 'success'
    });
  };

  const handleCloseSnackbar = () => {
    setSnackbar(prev => ({ ...prev, open: false }));
  };

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
        <Box sx={{ display: 'flex', alignItems: 'center' }}>
          <PeopleIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
          <Typography variant="h4" fontWeight="bold" gutterBottom>
            Network Peers
          </Typography>
        </Box>
        <Box>
          <Button 
            variant="contained" 
            startIcon={<AddIcon />} 
            onClick={() => setConnectDialogOpen(true)}
            sx={{ mr: 2 }}
          >
            Connect to Peer
          </Button>
          <IconButton 
            color="primary" 
            onClick={fetchPeers}
            disabled={loading}
          >
            <RefreshIcon />
          </IconButton>
        </Box>
      </Box>

      <Paper sx={{ p: 3, mb: 4 }}>
        <Typography variant="h6" gutterBottom>
          Connected Peers
        </Typography>
        <Typography variant="body2" color="text.secondary" paragraph>
          View and manage your connections to other blockchain nodes in the network.
        </Typography>

        {loading ? (
          <Box sx={{ display: 'flex', justifyContent: 'center', my: 4 }}>
            <CircularProgress />
          </Box>
        ) : (
          <TableContainer>
            <Table>
              <TableHead>
                <TableRow>
                  <TableCell>Node ID</TableCell>
                  <TableCell>Address</TableCell>
                  <TableCell>Port</TableCell>
                  <TableCell>Type</TableCell>
                  <TableCell>Actions</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {peers.length > 0 ? (
                  peers.map((peer) => (
                    <TableRow key={peer.id || `${peer.address}:${peer.port}`}>
                      <TableCell>{peer.id || 'Unknown'}</TableCell>
                      <TableCell>{peer.address}</TableCell>
                      <TableCell>{peer.port}</TableCell>
                      <TableCell>
                        <Chip 
                          label={peer.type === 'full' ? 'Full Node' : 'Wallet Node'} 
                          color={peer.type === 'full' ? 'primary' : 'secondary'}
                          size="small"
                        />
                      </TableCell>
                      <TableCell>
                        <IconButton 
                          color="error" 
                          size="small"
                          onClick={() => handleDisconnectPeer(peer.id)}
                        >
                          <DeleteIcon />
                        </IconButton>
                      </TableCell>
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={5} align="center">
                      <Typography variant="body1" sx={{ py: 2 }}>
                        No peers connected
                      </Typography>
                    </TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        )}
      </Paper>

      {/* Connect to Peer Dialog */}
      <Dialog open={connectDialogOpen} onClose={() => setConnectDialogOpen(false)}>
        <DialogTitle>Connect to Peer</DialogTitle>
        <DialogContent>
          <Grid container spacing={2} sx={{ mt: 1 }}>
            <Grid item xs={12}>
              <TextField
                label="Peer Address"
                fullWidth
                value={peerAddress}
                onChange={(e) => setPeerAddress(e.target.value)}
                placeholder="127.0.0.1"
              />
            </Grid>
            <Grid item xs={12}>
              <TextField
                label="Peer Port"
                fullWidth
                value={peerPort}
                onChange={(e) => setPeerPort(e.target.value)}
                placeholder="8000"
                type="number"
              />
            </Grid>
          </Grid>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setConnectDialogOpen(false)} color="inherit">
            Cancel
          </Button>
          <Button 
            onClick={handleConnectPeer} 
            color="primary" 
            variant="contained"
            startIcon={<LinkIcon />}
            disabled={loading}
          >
            Connect
          </Button>
        </DialogActions>
      </Dialog>

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

export default Peers; 