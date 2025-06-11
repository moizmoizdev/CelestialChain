import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Card, 
  CardContent, 
  CardHeader, 
  Typography, 
  Divider, 
  Grid,
  Paper,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Button,
  Chip,
  CircularProgress,
  Tabs,
  Tab
} from '@mui/material';
import AccountBoxIcon from '@mui/icons-material/AccountBox';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';
import { useNavigate } from 'react-router-dom';
import blockchainService from '../../api/blockchainService';

const AddressDetail = ({ address }) => {
  const navigate = useNavigate();
  const [loading, setLoading] = useState(true);
  const [addressData, setAddressData] = useState(null);
  const [error, setError] = useState(null);
  const [tabValue, setTabValue] = useState(0);

  useEffect(() => {
    const fetchAddressData = async () => {
      setLoading(true);
      try {
        const response = await blockchainService.getAddressDetails(address);
        setAddressData(response.data);
        setError(null);
      } catch (err) {
        console.error('Error fetching address details:', err);
        setError('Failed to fetch address details.');
      } finally {
        setLoading(false);
      }
    };

    if (address) {
      fetchAddressData();
    }
  }, [address]);

  const formatTimestamp = (timestamp) => {
    if (!timestamp) return 'N/A';
    const date = new Date(timestamp * 1000);
    return date.toLocaleString();
  };

  const truncateHash = (hash) => {
    if (!hash) return '';
    return hash.length > 20 
      ? `${hash.substring(0, 10)}...${hash.substring(hash.length - 10)}`
      : hash;
  };

  const handleTabChange = (event, newValue) => {
    setTabValue(newValue);
  };

  const handleViewTransaction = (hash) => {
    navigate(`/explorer/transaction/${hash}`);
  };

  const handleViewBlock = (blockNumber) => {
    navigate(`/explorer/block/${blockNumber}`);
  };

  if (loading) {
    return (
      <Box display="flex" justifyContent="center" alignItems="center" minHeight="300px">
        <CircularProgress />
      </Box>
    );
  }

  if (error) {
    return (
      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="Address Error" 
          sx={{ backgroundColor: 'error.main', color: 'white' }}
        />
        <CardContent>
          <Typography>{error}</Typography>
          <Button 
            variant="contained" 
            color="primary" 
            sx={{ mt: 2 }}
            onClick={() => navigate('/explorer')}
          >
            Back to Explorer
          </Button>
        </CardContent>
      </Card>
    );
  }

  // If we couldn't load the data, try to display minimal info
  if (!addressData) {
    return (
      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="Address Information" 
          sx={{ backgroundColor: 'primary.main', color: 'white' }}
        />
        <CardContent>
          <Typography variant="h6">Address: {address}</Typography>
          <Typography variant="body1">No detailed information available for this address.</Typography>
          <Button 
            variant="contained" 
            color="primary" 
            sx={{ mt: 2 }}
            onClick={() => navigate('/explorer')}
          >
            Back to Explorer
          </Button>
        </CardContent>
      </Card>
    );
  }

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', alignItems: 'center' }}>
        <AccountBoxIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
        <Typography variant="h4" fontWeight="bold">
          Address
        </Typography>
      </Box>

      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="Address Overview" 
          sx={{ backgroundColor: 'primary.main', color: 'white' }}
        />
        <CardContent>
          <Grid container spacing={2}>
            <Grid item xs={12}>
              <Typography variant="subtitle2" color="text.secondary">Address</Typography>
              <Paper 
                elevation={0} 
                sx={{ 
                  p: 1, 
                  backgroundColor: 'grey.100', 
                  wordBreak: 'break-all',
                  borderRadius: 1
                }}
              >
                <Typography variant="body2">{address}</Typography>
              </Paper>
            </Grid>

            <Grid item xs={12}>
              <Divider sx={{ my: 1 }} />
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Balance</Typography>
              <Typography variant="h5" fontWeight="bold" color="primary.main">
                {addressData.balance || 0} $CLST
              </Typography>
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Transaction Count</Typography>
              <Typography variant="h5" fontWeight="bold">
                {addressData.transactions?.length || 0}
              </Typography>
            </Grid>
          </Grid>
        </CardContent>
      </Card>

      {addressData.transactions && addressData.transactions.length > 0 && (
        <Card sx={{ borderRadius: 2 }}>
          <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
            <Tabs 
              value={tabValue} 
              onChange={handleTabChange}
              variant="fullWidth"
              textColor="primary"
              indicatorColor="primary"
            >
              <Tab icon={<SwapHorizIcon />} label="Transactions" id="transactions-tab" />
            </Tabs>
          </Box>

          <CardContent>
            <TableContainer component={Paper} sx={{ borderRadius: 0, mt: 2 }}>
              <Table>
                <TableHead sx={{ backgroundColor: 'grey.100' }}>
                  <TableRow>
                    <TableCell width="25%">Transaction Hash</TableCell>
                    <TableCell width="15%">Block</TableCell>
                    <TableCell width="20%">Type</TableCell>
                    <TableCell width="15%">Amount</TableCell>
                    <TableCell width="15%">Date</TableCell>
                    <TableCell width="10%">Actions</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {addressData.transactions.map((tx, index) => {
                    const isSender = tx.sender === address;
                    return (
                      <TableRow key={tx.hash || index} hover>
                        <TableCell>
                          <Typography variant="body2" sx={{ wordBreak: 'break-all' }}>
                            {truncateHash(tx.hash)}
                          </Typography>
                        </TableCell>
                        <TableCell>
                          {tx.blockHeight || tx.blockNumber ? (
                            <Button 
                              size="small" 
                              variant="text"
                              onClick={() => handleViewBlock(tx.blockHeight || tx.blockNumber)}
                            >
                              #{tx.blockHeight || tx.blockNumber}
                            </Button>
                          ) : (
                            <Chip label="Pending" size="small" color="warning" />
                          )}
                        </TableCell>
                        <TableCell>
                          {tx.sender === 'Genesis' ? (
                            <Chip label="Mining Reward" size="small" color="success" />
                          ) : (
                            <Chip 
                              label={isSender ? 'Sent' : 'Received'} 
                              size="small" 
                              color={isSender ? 'error' : 'success'} 
                            />
                          )}
                        </TableCell>
                        <TableCell>
                          <Typography 
                            color={isSender ? 'error.main' : 'success.main'}
                            fontWeight="medium"
                          >
                            {isSender ? '-' : '+'}{tx.amount} $CLST
                          </Typography>
                        </TableCell>
                        <TableCell>{formatTimestamp(tx.timestamp)}</TableCell>
                        <TableCell>
                          <Button
                            variant="contained"
                            size="small"
                            onClick={() => handleViewTransaction(tx.hash)}
                          >
                            View
                          </Button>
                        </TableCell>
                      </TableRow>
                    );
                  })}
                </TableBody>
              </Table>
            </TableContainer>
          </CardContent>
        </Card>
      )}

      <Box sx={{ mt: 2, mb: 4 }}>
        <Button 
          variant="outlined" 
          color="primary" 
          onClick={() => navigate('/explorer')}
        >
          Back to Explorer
        </Button>
      </Box>
    </Box>
  );
};

export default AddressDetail; 