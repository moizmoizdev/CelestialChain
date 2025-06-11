import React from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Grid, 
  Chip,
  Tooltip,
  IconButton
} from '@mui/material';
import ArrowUpwardIcon from '@mui/icons-material/ArrowUpward';
import ArrowDownwardIcon from '@mui/icons-material/ArrowDownward';
import ContentCopyIcon from '@mui/icons-material/ContentCopy';
import LaunchIcon from '@mui/icons-material/Launch';
import { useNavigate } from 'react-router-dom';

const WalletTransactionItem = ({ transaction, walletAddress, onCopy }) => {
  const navigate = useNavigate();
  const isSender = transaction.sender === walletAddress;
  
  const truncateAddress = (address) => {
    if (!address) return '';
    return address.length > 20 
      ? `${address.substring(0, 10)}...${address.substring(address.length - 10)}`
      : address;
  };
  
  const formatTimestamp = (timestamp) => {
    if (!timestamp) return 'N/A';
    const date = new Date(timestamp * 1000);
    return date.toLocaleString();
  };
  
  const handleCopy = (text) => {
    if (onCopy) {
      onCopy(text);
    } else {
      navigator.clipboard.writeText(text);
    }
  };
  
  const handleViewTransaction = () => {
    if (transaction.hash) {
      navigate(`/explorer/transaction/${transaction.hash}`);
    }
  };

  // Determine if transaction is confirmed by checking different possible properties
  const isConfirmed = () => {
    // Check for common properties that indicate a confirmed transaction
    return (
      transaction.blockHeight !== undefined && transaction.blockHeight !== null ||
      transaction.blockNumber !== undefined && transaction.blockNumber !== null ||
      transaction.status === 'confirmed' || 
      transaction.status === 'Confirmed' ||
      transaction.confirmed === true
    );
  };
  
  // Get block number from various potential properties
  const getBlockNumber = () => {
    if (transaction.blockHeight !== undefined) return transaction.blockHeight;
    if (transaction.blockNumber !== undefined) return transaction.blockNumber;
    return null;
  };
  
  const confirmed = isConfirmed();
  const blockNumber = getBlockNumber();
  
  return (
    <Paper 
      sx={{ 
        p: 2, 
        mb: 2, 
        borderRadius: 2,
        borderLeft: '4px solid',
        borderColor: isSender ? 'error.main' : 'success.main',
      }}
    >
      <Grid container spacing={2} alignItems="center">
        <Grid item xs={1}>
          <Box 
            sx={{ 
              backgroundColor: isSender ? 'error.light' : 'success.light',
              color: isSender ? 'error.contrastText' : 'success.contrastText',
              width: 40,
              height: 40,
              borderRadius: '50%',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center'
            }}
          >
            {isSender 
              ? <ArrowUpwardIcon fontSize="small" /> 
              : <ArrowDownwardIcon fontSize="small" />
            }
          </Box>
        </Grid>
        
        <Grid item xs={11} sm={5}>
          <Typography variant="body2" color="text.secondary">
            {isSender ? 'To:' : 'From:'}
          </Typography>
          <Box display="flex" alignItems="center">
            <Typography variant="body1" sx={{ mr: 1 }}>
              {truncateAddress(isSender ? transaction.receiver : transaction.sender)}
            </Typography>
            <Tooltip title="Copy Address">
              <IconButton 
                size="small"
                onClick={() => handleCopy(isSender ? transaction.receiver : transaction.sender)}
              >
                <ContentCopyIcon fontSize="small" />
              </IconButton>
            </Tooltip>
          </Box>
        </Grid>
        
        <Grid item xs={6} sm={3}>
          <Typography variant="body2" color="text.secondary">
            Amount
          </Typography>
          <Typography 
            variant="body1" 
            fontWeight="bold"
            color={isSender ? 'error.main' : 'success.main'}
          >
            {isSender ? '-' : '+'}{transaction.amount} $CLST
          </Typography>
        </Grid>
        
        <Grid item xs={6} sm={3}>
          <Box display="flex" flexDirection="column" alignItems="flex-end">
            <Box display="flex" alignItems="center" mb={1}>
              <Chip 
                label={confirmed ? 'Confirmed' : 'Pending'} 
                size="small"
                color={confirmed ? 'success' : 'warning'}
                sx={{ mr: 1 }}
              />
              {transaction.hash && (
                <Tooltip title="View Transaction Details">
                  <IconButton 
                    size="small"
                    onClick={handleViewTransaction}
                    color="primary"
                  >
                    <LaunchIcon fontSize="small" />
                  </IconButton>
                </Tooltip>
              )}
            </Box>
            {blockNumber && (
              <Typography variant="caption" color="text.secondary">
                Block #{blockNumber}
              </Typography>
            )}
            {transaction.timestamp && (
              <Typography variant="caption" color="text.secondary">
                {formatTimestamp(transaction.timestamp)}
              </Typography>
            )}
          </Box>
        </Grid>
      </Grid>
    </Paper>
  );
};

export default WalletTransactionItem; 