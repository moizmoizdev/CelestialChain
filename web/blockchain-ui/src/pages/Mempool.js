import React, { useState, useEffect } from 'react';
import { Box, Typography, Paper, Skeleton, Alert, Grid, Button } from '@mui/material';
import { useNavigate } from 'react-router-dom';
import blockchainService from '../api/blockchainService';
import RefreshIcon from '@mui/icons-material/Refresh';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';

// Components
import TransactionCard from '../components/UI/TransactionCard';

const Mempool = () => {
  const navigate = useNavigate();
  const [transactions, setTransactions] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  const fetchMempoolTransactions = async () => {
    try {
      setLoading(true);
      setError(null);
      
      const response = await blockchainService.getMempool();
      console.log('Mempool response:', response);
      
      if (response.data && response.data.transactions) {
        // If we get an array of transactions
        setTransactions(response.data.transactions);
      } else if (response.data && Array.isArray(response.data)) {
        // If the API returns the transactions directly as an array
        setTransactions(response.data);
      } else {
        // Handle case where we got a response but no transactions
        setTransactions([]);
        console.warn('No transactions found in mempool response', response);
      }
    } catch (error) {
      console.error('Error fetching mempool:', error);
      setError('Failed to load mempool transactions. Please try again later.');
      
      // Set empty transactions array on error
      setTransactions([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchMempoolTransactions();
    
    // Poll for updates every 10 seconds
    const interval = setInterval(() => {
      fetchMempoolTransactions();
    }, 10000);
    
    return () => clearInterval(interval);
  }, []);

  const handleRefresh = () => {
    fetchMempoolTransactions();
  };

  const handleCreateTransaction = () => {
    navigate('/transactions');
  };

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <Box>
          <Typography variant="h4" fontWeight="bold" gutterBottom>
            Mempool
          </Typography>
          <Typography variant="body1" color="text.secondary">
            View pending transactions waiting to be included in blocks.
          </Typography>
        </Box>
        
        <Box>
          <Button 
            variant="outlined" 
            startIcon={<RefreshIcon />} 
            onClick={handleRefresh}
            sx={{ mr: 2 }}
          >
            Refresh
          </Button>
          <Button 
            variant="contained" 
            startIcon={<SwapHorizIcon />}
            onClick={handleCreateTransaction}
          >
            Create Transaction
          </Button>
        </Box>
      </Box>
      
      <Paper sx={{ p: 3, borderRadius: 2, mb: 4 }}>
        {loading ? (
          // Show skeletons while loading
          Array(3).fill(0).map((_, index) => (
            <Skeleton 
              key={index} 
              variant="rectangular" 
              height={160} 
              sx={{ mb: 2, borderRadius: 2 }} 
              animation="wave" 
            />
          ))
        ) : error ? (
          // Show error message
          <Alert severity="error">{error}</Alert>
        ) : transactions.length > 0 ? (
          // Show transactions
          <Box>
            <Typography variant="h6" gutterBottom>
              Pending Transactions ({transactions.length})
            </Typography>
            <Grid container spacing={3} sx={{ mt: 1 }}>
              {transactions.map((tx) => (
                <Grid item xs={12} md={6} key={tx.hash}>
                  <TransactionCard 
                    transaction={{
                      ...tx,
                      status: 'Pending'
                    }} 
                    onClick={() => navigate(`/explorer/transaction/${tx.hash}`)}
                  />
                </Grid>
              ))}
            </Grid>
          </Box>
        ) : (
          // No transactions
          <Box sx={{ textAlign: 'center', py: 4 }}>
            <Typography variant="h6" gutterBottom>
              No Pending Transactions
            </Typography>
            <Typography variant="body1" color="text.secondary" paragraph>
              The mempool is currently empty. Create a new transaction to see it here.
            </Typography>
            <Button 
              variant="contained" 
              startIcon={<SwapHorizIcon />}
              onClick={handleCreateTransaction}
              sx={{ mt: 2 }}
            >
              Create Transaction
            </Button>
          </Box>
        )}
      </Paper>
    </Box>
  );
};

export default Mempool; 