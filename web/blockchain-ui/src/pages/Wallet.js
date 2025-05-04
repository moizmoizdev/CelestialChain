import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Grid, 
  Button, 
  TextField, 
  CircularProgress,
  Divider,
  IconButton,
  Tooltip,
  Snackbar,
  Alert,
  Card,
  CardContent 
} from '@mui/material';
import ContentCopyIcon from '@mui/icons-material/ContentCopy';
import SendIcon from '@mui/icons-material/Send';
import AccountBalanceWalletIcon from '@mui/icons-material/AccountBalanceWallet';
import WalletTransactionItem from '../components/UI/WalletTransactionItem';
import blockchainService from '../api/blockchainService';

const Wallet = () => {
  const [walletInfo, setWalletInfo] = useState({
    address: '',
    balance: 0
  });
  const [receiver, setReceiver] = useState('');
  const [amount, setAmount] = useState('');
  const [loading, setLoading] = useState(false);
  const [txLoading, setTxLoading] = useState(false);
  const [transactions, setTransactions] = useState([]);
  const [alertInfo, setAlertInfo] = useState({
    open: false,
    message: '',
    severity: 'success'
  });

  useEffect(() => {
    fetchWalletData();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  // Helper function to normalize transaction data
  const processTransactions = (txs, walletAddress) => {
    if (!txs || !Array.isArray(txs)) return [];
    
    return txs.map(tx => {
      // Check if the transaction belongs to this wallet
      if (tx.sender !== walletAddress && tx.receiver !== walletAddress) {
        return null;
      }
      
      // Check if the transaction has block information
      const isConfirmed = tx.blockHeight !== undefined || 
                          tx.blockNumber !== undefined || 
                          tx.status === 'confirmed' ||
                          tx.status === 'Confirmed';
      
      // If not confirmed but tx has a hash, it's likely a valid pending transaction
      const isPending = !isConfirmed && tx.hash;
      
      // Only include transactions that are confirmed or pending
      if (!isConfirmed && !isPending) return null;
      
      return {
        ...tx,
        // Ensure key fields have values
        sender: tx.sender || 'Unknown',
        receiver: tx.receiver || 'Unknown',
        amount: typeof tx.amount === 'number' ? tx.amount : parseFloat(tx.amount) || 0,
        hash: tx.hash || `temp-${Math.random().toString(36).substring(7)}`,
        // Add status field if not already present
        status: isConfirmed ? 'Confirmed' : 'Pending'
      };
    }).filter(tx => tx !== null);
  };

  const fetchWalletData = async () => {
    setLoading(true);
    try {
      // Get wallet information
      const walletResponse = await blockchainService.getWalletInfo();
      setWalletInfo(walletResponse.data);
      
      // Get transaction history
      try {
        const txResponse = await blockchainService.getLatestTransactions(10);
        let txData = [];
        
        // Handle different API response formats
        if (txResponse.data && Array.isArray(txResponse.data)) {
          txData = txResponse.data;
        } else if (txResponse.data && txResponse.data.transactions && Array.isArray(txResponse.data.transactions)) {
          txData = txResponse.data.transactions;
        }
        
        // Also try to get mempool transactions for pending ones
        try {
          const mempoolResponse = await blockchainService.getMempool();
          if (mempoolResponse.data && Array.isArray(mempoolResponse.data)) {
            // Add pending flag to mempool transactions
            const pendingTxs = mempoolResponse.data.map(tx => ({
              ...tx,
              status: 'Pending'
            }));
            txData = [...txData, ...pendingTxs];
          }
        } catch (mempoolError) {
          console.warn('Failed to fetch mempool transactions:', mempoolError);
        }
        
        // Process and filter transactions for this wallet
        const processedTransactions = processTransactions(txData, walletResponse.data.address);
        
        // Sort by timestamp (newer first)
        processedTransactions.sort((a, b) => {
          const timeA = a.timestamp || 0;
          const timeB = b.timestamp || 0;
          return timeB - timeA;
        });
        
        setTransactions(processedTransactions);
      } catch (txError) {
        console.error('Error fetching transactions:', txError);
        setTransactions([]);
      }
    } catch (error) {
      console.error('Error fetching wallet data:', error);
      showAlert('Failed to load wallet data', 'error');
    } finally {
      setLoading(false);
    }
  };

  const handleCreateTransaction = async () => {
    if (!receiver || !amount || amount <= 0) {
      showAlert('Please enter a valid receiver address and amount', 'error');
      return;
    }

    setTxLoading(true);
    try {
      const response = await blockchainService.createTransaction(receiver, parseFloat(amount));
      setReceiver('');
      setAmount('');
      
      // Add the new transaction to the list with Pending status
      if (response && response.data) {
        const newTx = {
          sender: walletInfo.address,
          receiver: receiver,
          amount: parseFloat(amount),
          hash: response.data.hash || `pending-${Date.now()}`,
          timestamp: Math.floor(Date.now() / 1000),
          status: 'Pending'
        };
        
        setTransactions([newTx, ...transactions]);
      }
      
      showAlert('Transaction created successfully!', 'success');
      
      // Refresh wallet data to update balance and transactions
      setTimeout(() => {
        fetchWalletData();
      }, 2000);
    } catch (error) {
      console.error('Error creating transaction:', error);
      showAlert('Error creating transaction: ' + (error.response?.data?.message || error.message), 'error');
    } finally {
      setTxLoading(false);
    }
  };

  const copyToClipboard = (text) => {
    navigator.clipboard.writeText(text);
    showAlert('Copied to clipboard!', 'success');
  };

  const showAlert = (message, severity) => {
    setAlertInfo({
      open: true,
      message,
      severity
    });
  };

  const handleCloseAlert = () => {
    setAlertInfo({
      ...alertInfo,
      open: false
    });
  };

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', alignItems: 'center' }}>
        <AccountBalanceWalletIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
        <Typography variant="h4" fontWeight="bold" gutterBottom>
          My Wallet
        </Typography>
      </Box>

      {loading ? (
        <Box display="flex" justifyContent="center" my={4}>
          <CircularProgress />
        </Box>
      ) : (
        <Grid container spacing={3}>
          {/* Wallet Info Card */}
          <Grid item xs={12} md={5}>
            <Paper sx={{ p: 3, borderRadius: 2, height: '100%' }}>
              <Typography variant="h6" fontWeight="bold" gutterBottom>
                Wallet Details
              </Typography>
              <Divider sx={{ my: 2 }} />
              
              <Box sx={{ mb: 3 }}>
                <Typography variant="subtitle2" color="text.secondary">
                  Wallet Address
                </Typography>
                <Box display="flex" alignItems="center">
                  <Typography variant="body1" sx={{ wordBreak: 'break-all', mr: 1 }}>
                    {walletInfo.address}
                  </Typography>
                  <Tooltip title="Copy Address">
                    <IconButton 
                      size="small" 
                      onClick={() => copyToClipboard(walletInfo.address)}
                    >
                      <ContentCopyIcon fontSize="small" />
                    </IconButton>
                  </Tooltip>
                </Box>
              </Box>
              
              <Box>
                <Typography variant="subtitle2" color="text.secondary">
                  Current Balance
                </Typography>
                <Typography variant="h4" fontWeight="bold" color="primary.main">
                  {walletInfo.balance} $CLST
                </Typography>
              </Box>
              
              <Button 
                variant="outlined" 
                color="primary" 
                sx={{ mt: 3 }}
                onClick={fetchWalletData}
              >
                Refresh Balance
              </Button>
            </Paper>
          </Grid>
          
          {/* Send Money Form */}
          <Grid item xs={12} md={7}>
            <Paper sx={{ p: 3, borderRadius: 2 }}>
              <Box display="flex" alignItems="center" mb={2}>
                <SendIcon sx={{ mr: 1, color: 'primary.main' }} />
                <Typography variant="h6" fontWeight="bold">
                  Send Coins
                </Typography>
              </Box>
              <Divider sx={{ mb: 3 }} />
              
              <Grid container spacing={2}>
                <Grid item xs={12}>
                  <TextField
                    label="Receiver Address"
                    value={receiver}
                    onChange={(e) => setReceiver(e.target.value)}
                    fullWidth
                    required
                    placeholder="0x..."
                  />
                </Grid>
                <Grid item xs={12} sm={6}>
                  <TextField
                    label="Amount ($CLST)"
                    value={amount}
                    onChange={(e) => setAmount(e.target.value)}
                    type="number"
                    inputProps={{ min: 0, step: 0.01 }}
                    fullWidth
                    required
                  />
                </Grid>
                <Grid item xs={12} sm={6} display="flex" alignItems="flex-end">
                  <Button
                    variant="contained"
                    color="primary"
                    fullWidth
                    size="large"
                    onClick={handleCreateTransaction}
                    disabled={txLoading || !receiver || !amount}
                    startIcon={txLoading ? <CircularProgress size={20} color="inherit" /> : <SendIcon />}
                  >
                    Send
                  </Button>
                </Grid>
              </Grid>
            </Paper>
          </Grid>
          
          {/* Transaction History */}
          <Grid item xs={12}>
            <Card sx={{ borderRadius: 2 }}>
              <CardContent>
                <Typography variant="h6" fontWeight="bold" gutterBottom>
                  Recent Transactions
                </Typography>
                <Divider sx={{ mb: 2 }} />
                
                {transactions.length > 0 ? (
                  <Box>
                    {transactions.map((tx, index) => (
                      <WalletTransactionItem 
                        key={tx.hash || index}
                        transaction={tx}
                        walletAddress={walletInfo.address}
                        onCopy={copyToClipboard}
                      />
                    ))}
                  </Box>
                ) : (
                  <Typography variant="body1" color="text.secondary" align="center" sx={{ py: 3 }}>
                    No transactions found for this wallet
                  </Typography>
                )}
              </CardContent>
            </Card>
          </Grid>
        </Grid>
      )}
      
      <Snackbar
        open={alertInfo.open}
        autoHideDuration={6000}
        onClose={handleCloseAlert}
        anchorOrigin={{ vertical: 'bottom', horizontal: 'center' }}
      >
        <Alert 
          onClose={handleCloseAlert} 
          severity={alertInfo.severity}
          sx={{ width: '100%' }}
        >
          {alertInfo.message}
        </Alert>
      </Snackbar>
    </Box>
  );
};

export default Wallet; 