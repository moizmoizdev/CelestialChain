import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Card, 
  CardContent, 
  CardHeader, 
  Typography, 
  Divider, 
  Grid,
  Button,
  Chip,
  CircularProgress,
  Paper
} from '@mui/material';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';
import { useNavigate } from 'react-router-dom';
import blockchainService from '../../api/blockchainService';

const TransactionDetail = ({ hash }) => {
  const navigate = useNavigate();
  const [loading, setLoading] = useState(true);
  const [transaction, setTransaction] = useState(null);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchTransactionData = async () => {
      setLoading(true);
      try {
        const response = await blockchainService.getTransactionByHash(hash);
        setTransaction(response.data);
        setError(null);
      } catch (err) {
        console.error('Error fetching transaction:', err);
        setError('Failed to fetch transaction data. The transaction may not exist.');
      } finally {
        setLoading(false);
      }
    };

    if (hash) {
      fetchTransactionData();
    }
  }, [hash]);

  const formatTimestamp = (timestamp) => {
    if (!timestamp) return 'N/A';
    const date = new Date(timestamp * 1000);
    return date.toLocaleString();
  };

  const handleViewBlock = (blockNumber) => {
    navigate(`/explorer/block/${blockNumber}`);
  };

  const handleViewAddress = (address) => {
    navigate(`/explorer/address/${address}`);
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
          title="Transaction Not Found" 
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

  if (!transaction) {
    return (
      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="No Transaction Data" 
          sx={{ backgroundColor: 'warning.main', color: 'white' }}
        />
        <CardContent>
          <Typography>No data available for this transaction.</Typography>
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
        <SwapHorizIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
        <Typography variant="h4" fontWeight="bold">
          Transaction Details
        </Typography>
      </Box>

      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="Transaction Information" 
          sx={{ backgroundColor: 'primary.main', color: 'white' }}
        />
        <CardContent>
          <Grid container spacing={2}>
            <Grid item xs={12}>
              <Typography variant="subtitle2" color="text.secondary">Transaction Hash</Typography>
              <Paper 
                elevation={0} 
                sx={{ 
                  p: 1, 
                  backgroundColor: 'grey.100', 
                  wordBreak: 'break-all',
                  overflowWrap: 'break-word',
                  borderRadius: 1
                }}
              >
                <Typography variant="body2">{transaction.hash}</Typography>
              </Paper>
            </Grid>

            <Grid item xs={12}>
              <Divider sx={{ my: 1 }} />
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Status</Typography>
              <Typography variant="body1">
                {transaction.blockHeight || transaction.blockNumber ? (
                  <Chip 
                    label="Confirmed" 
                    color="success" 
                    size="small" 
                    sx={{ fontSize: '0.875rem' }}
                  />
                ) : (
                  <Chip 
                    label="Pending" 
                    color="warning" 
                    size="small" 
                    sx={{ fontSize: '0.875rem' }}
                  />
                )}
              </Typography>
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Block</Typography>
              <Typography variant="body1">
                {transaction.blockHeight || transaction.blockNumber ? (
                  <Button 
                    variant="text" 
                    color="primary"
                    onClick={() => handleViewBlock(transaction.blockHeight || transaction.blockNumber)}
                  >
                    #{transaction.blockHeight || transaction.blockNumber}
                  </Button>
                ) : (
                  'Pending'
                )}
              </Typography>
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Timestamp</Typography>
              <Typography variant="body1">{formatTimestamp(transaction.timestamp)}</Typography>
            </Grid>

            <Grid item xs={12}>
              <Divider sx={{ my: 1 }} />
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">From</Typography>
              {transaction.sender === 'Genesis' ? (
                <Chip label="Genesis (Mining Reward)" color="success" />
              ) : (
                <Button 
                  variant="text" 
                  color="primary"
                  onClick={() => handleViewAddress(transaction.sender)}
                  sx={{ 
                    textAlign: 'left', 
                    textTransform: 'none', 
                    wordBreak: 'break-all', 
                    justifyContent: 'flex-start' 
                  }}
                >
                  {transaction.sender}
                </Button>
              )}
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">To</Typography>
              <Button 
                variant="text" 
                color="primary"
                onClick={() => handleViewAddress(transaction.receiver)}
                sx={{ 
                  textAlign: 'left', 
                  textTransform: 'none', 
                  wordBreak: 'break-all', 
                  justifyContent: 'flex-start' 
                }}
              >
                {transaction.receiver}
              </Button>
            </Grid>

            <Grid item xs={12}>
              <Divider sx={{ my: 1 }} />
            </Grid>

            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Value</Typography>
              <Typography variant="h5" fontWeight="bold" color="primary.main">
                {transaction.amount} $CLST
              </Typography>
            </Grid>
          </Grid>
        </CardContent>
      </Card>

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

export default TransactionDetail; 