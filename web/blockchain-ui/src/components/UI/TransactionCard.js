import React from 'react';
import { 
  Card, 
  Box, 
  Typography, 
  Chip, 
  IconButton, 
  Tooltip,
  Divider,
  useTheme
} from '@mui/material';
import { styled } from '@mui/material/styles';
import OpenInNewIcon from '@mui/icons-material/OpenInNew';
import ContentCopyIcon from '@mui/icons-material/ContentCopy';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import HourglassEmptyIcon from '@mui/icons-material/HourglassEmpty';

const StyledCard = styled(Card)(({ theme }) => ({
  padding: theme.spacing(2.5),
  borderRadius: theme.shape.borderRadius,
  backgroundColor: theme.palette.background.card,
  transition: 'transform 0.3s ease, box-shadow 0.3s ease',
  boxShadow: '0 4px 20px rgba(0, 0, 0, 0.1)',
  '&:hover': {
    transform: 'translateY(-3px)',
    boxShadow: '0 8px 24px rgba(0, 0, 0, 0.15)',
  },
}));

const AddressChip = styled(Box)(({ theme }) => ({
  display: 'flex',
  alignItems: 'center',
  padding: theme.spacing(0.75, 1.5),
  borderRadius: 20,
  backgroundColor: theme.palette.background.paper,
  maxWidth: '100%',
  overflow: 'hidden',
  whiteSpace: 'nowrap',
  textOverflow: 'ellipsis',
}));

const truncateAddress = (address, length = 8) => {
  if (!address) return '';
  return `${address.substring(0, length)}...${address.substring(address.length - length)}`;
};

// Function to copy text to clipboard
const copyToClipboard = (text) => {
  navigator.clipboard.writeText(text);
};

const TransactionCard = ({ transaction, onClick }) => {
  const theme = useTheme();
  const { hash, sender, receiver, amount, status, blockNumber, timestamp } = transaction;
  
  // Format timestamp if available
  const formattedTime = timestamp ? new Date(timestamp * 1000).toLocaleString() : 'N/A';
  
  return (
    <StyledCard onClick={onClick}>
      {/* Transaction Hash */}
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
        <Box>
          <Typography variant="subtitle2" color="text.secondary">
            Transaction
          </Typography>
          <Box sx={{ display: 'flex', alignItems: 'center' }}>
            <Typography variant="body2" fontFamily="monospace" fontWeight="medium">
              {truncateAddress(hash, 15)}
            </Typography>
            <Tooltip title="Copy hash">
              <IconButton size="small" onClick={(e) => {
                e.stopPropagation();
                copyToClipboard(hash);
              }}>
                <ContentCopyIcon fontSize="small" />
              </IconButton>
            </Tooltip>
          </Box>
        </Box>
        
        <Box>
          <Chip 
            icon={status === 'Confirmed' ? <CheckCircleIcon /> : <HourglassEmptyIcon />}
            label={status}
            size="small"
            color={status === 'Confirmed' ? 'success' : 'warning'}
            variant="outlined"
          />
        </Box>
      </Box>
      
      {/* Transaction Details */}
      <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
        {/* From address */}
        <Box sx={{ flex: 1, mr: 1 }}>
          <Typography variant="caption" color="text.secondary" mb={0.5}>
            From
          </Typography>
          <AddressChip>
            <Typography variant="body2" fontFamily="monospace" noWrap>
              {truncateAddress(sender)}
            </Typography>
            <Tooltip title="Copy address">
              <IconButton size="small" sx={{ ml: 0.5 }} onClick={(e) => {
                e.stopPropagation();
                copyToClipboard(sender);
              }}>
                <ContentCopyIcon fontSize="small" />
              </IconButton>
            </Tooltip>
          </AddressChip>
        </Box>
        
        {/* Arrow */}
        <Box sx={{ mx: 1, display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
          <ArrowForwardIcon color="primary" />
        </Box>
        
        {/* To address */}
        <Box sx={{ flex: 1 }}>
          <Typography variant="caption" color="text.secondary" mb={0.5}>
            To
          </Typography>
          <AddressChip>
            <Typography variant="body2" fontFamily="monospace" noWrap>
              {truncateAddress(receiver)}
            </Typography>
            <Tooltip title="Copy address">
              <IconButton size="small" sx={{ ml: 0.5 }} onClick={(e) => {
                e.stopPropagation();
                copyToClipboard(receiver);
              }}>
                <ContentCopyIcon fontSize="small" />
              </IconButton>
            </Tooltip>
          </AddressChip>
        </Box>
      </Box>
      
      <Divider sx={{ my: 2 }} />
      
      {/* Footer - Amount and details */}
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <Box>
          <Typography variant="body2" color="text.secondary">
            Amount
          </Typography>
          <Typography variant="h6" fontWeight="bold" color="primary.main">
            {amount} $CLST
          </Typography>
        </Box>
        
        <Box sx={{ textAlign: 'right' }}>
          {blockNumber && (
            <Typography variant="body2" color="text.secondary">
              Block: {blockNumber}
            </Typography>
          )}
          <Typography variant="caption" color="text.secondary">
            {formattedTime}
          </Typography>
        </Box>
      </Box>
    </StyledCard>
  );
};

export default TransactionCard; 