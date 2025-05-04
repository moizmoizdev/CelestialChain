import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  TextField, 
  Button, 
  Grid, 
  Divider, 
  Alert,
  Skeleton,
  Tab,
  Tabs
} from '@mui/material';
import SendIcon from '@mui/icons-material/Send';
import HistoryIcon from '@mui/icons-material/History';
import blockchainService from '../api/blockchainService';
import TransactionCard from '../components/UI/TransactionCard';
import { useNavigate } from 'react-router-dom';

const Transactions = () => {
  const navigate = useNavigate();
  const [tabValue, setTabValue] = useState(0);
  const [receiverAddress, setReceiverAddress] = useState('');
  const [amount, setAmount] = useState('');
  const [transactions, setTransactions] = useState([]);
  const [loading, setLoading] = useState(false);
  const [createLoading, setCreateLoading] = useState(false);
  const [error, setError] = useState(null);
  const [success, setSuccess] = useState(null);
  
  // Fetch transaction history on component mount
  useEffect(() => {
    fetchTransactionHistory();
  }, []);
  
  const fetchTransactionHistory = async () => {
    try {
      setLoading(true);
      setError(null);
      
      const response = await blockchainService.getLatestTransactions(20);
      console.log('Transaction history response:', response);
      
      if (response.data && response.data.transactions) {
        setTransactions(response.data.transactions);
      } else if (response.data && Array.isArray(response.data)) {
        setTransactions(response.data);
      } else {
        setTransactions([]);
      }
    } catch (error) {
      console.error('Error fetching transaction history:', error);
      setError('Failed to load transaction history');
    } finally {
      setLoading(false);
    }
  };
  
  const handleTabChange = (event, newValue) => {
    setTabValue(newValue);
  };
  
  const handleCreateTransaction = async (e) => {
    e.preventDefault();
    
    if (!receiverAddress || !amount) {
      setError('Receiver address and amount are required');
      return;
    }
    
    try {
      setCreateLoading(true);
      setError(null);
      setSuccess(null);
      
      // Validate amount is a positive number
      const amountValue = parseFloat(amount);
      if (isNaN(amountValue) || amountValue <= 0) {
        setError('Amount must be a positive number');
        setCreateLoading(false);
        return;
      }
      
      console.log('Creating transaction:', { receiverAddress, amount: amountValue });
      
      const response = await blockchainService.createTransaction(receiverAddress, amountValue);
      console.log('Transaction creation response:', response);
      
      // Display success message
      setSuccess('Transaction created successfully and sent to the network.');
      
      // Clear form
      setReceiverAddress('');
      setAmount('');
      
      // Refresh transaction history
      fetchTransactionHistory();
      
      // Switch to Transaction History tab
      setTabValue(1);
      
    } catch (error) {
      console.error('Error creating transaction:', error);
      setError(error.response?.data?.error || 'Failed to create transaction. Please try again.');
    } finally {
      setCreateLoading(false);
    }
  };

  return (
    <Box>
      <Box sx={{ mb: 4 }}>
        <Typography variant="h4" fontWeight="bold" gutterBottom>
          Transactions
        </Typography>
        <Typography variant="body1" color="text.secondary">
          Create new transactions and view transaction history.
        </Typography>
      </Box>
      
      <Paper sx={{ p: 3, borderRadius: 2, mb: 4 }}>
        <Tabs value={tabValue} onChange={handleTabChange} sx={{ mb: 3 }}>
          <Tab label="Create Transaction" icon={<SendIcon />} iconPosition="start" />
          <Tab label="Transaction History" icon={<HistoryIcon />} iconPosition="start" />
        </Tabs>
        
        <Divider sx={{ mb: 3 }} />
        
        {/* Create Transaction Tab */}
        <Box sx={{ display: tabValue === 0 ? 'block' : 'none' }}>
          {error && (
            <Alert severity="error" sx={{ mb: 3 }}>{error}</Alert>
          )}
          
          {success && (
            <Alert severity="success" sx={{ mb: 3 }}>{success}</Alert>
          )}
          
          <form onSubmit={handleCreateTransaction}>
            <Grid container spacing={3}>
              <Grid item xs={12}>
                <TextField
                  label="Receiver Address"
                  fullWidth
                  placeholder="Enter the receiver's wallet address (0x...)"
                  value={receiverAddress}
                  onChange={(e) => setReceiverAddress(e.target.value)}
                  required
                />
              </Grid>
              
              <Grid item xs={12} md={6}>
                <TextField
                  label="Amount"
                  fullWidth
                  type="number"
                  placeholder="Amount to send"
                  value={amount}
                  onChange={(e) => setAmount(e.target.value)}
                  required
                  InputProps={{
                    endAdornment: <Typography variant="body2">$CLST</Typography>
                  }}
                  inputProps={{
                    step: 0.01,
                    min: 0.01
                  }}
                />
              </Grid>
              
              <Grid item xs={12}>
                <Button
                  type="submit"
                  variant="contained"
                  size="large"
                  startIcon={<SendIcon />}
                  disabled={createLoading}
                  sx={{ mt: 2 }}
                >
                  {createLoading ? 'Sending...' : 'Send Transaction'}
                </Button>
              </Grid>
            </Grid>
          </form>
        </Box>
        
        {/* Transaction History Tab */}
        <Box sx={{ display: tabValue === 1 ? 'block' : 'none' }}>
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
            <Alert severity="error">{error}</Alert>
          ) : transactions.length > 0 ? (
            <Grid container spacing={3}>
              {transactions.map((tx) => (
                <Grid item xs={12} md={6} key={tx.hash}>
                  <TransactionCard 
                    transaction={tx} 
                    onClick={() => navigate(`/explorer/transaction/${tx.hash}`)}
                  />
                </Grid>
              ))}
            </Grid>
          ) : (
            <Box sx={{ textAlign: 'center', py: 4 }}>
              <Typography variant="h6" gutterBottom>
                No Transactions Found
              </Typography>
              <Typography variant="body1" color="text.secondary">
                You haven't made any transactions yet. Create a new transaction to get started.
              </Typography>
            </Box>
          )}
        </Box>
      </Paper>
    </Box>
  );
};

export default Transactions; 