import React, { useState } from 'react';
import { 
  Card, 
  Box, 
  Typography, 
  IconButton, 
  Tooltip, 
  Divider, 
  Collapse,
  Chip,
  useTheme,
  Stack
} from '@mui/material';
import { styled } from '@mui/material/styles';
import ContentCopyIcon from '@mui/icons-material/ContentCopy';
import KeyboardArrowDownIcon from '@mui/icons-material/KeyboardArrowDown';
import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import CubeIcon from '@mui/icons-material/ViewInAr';

const StyledCard = styled(Card)(({ theme }) => ({
  padding: theme.spacing(2.5),
  borderRadius: theme.shape.borderRadius,
  backgroundColor: theme.palette.background.card,
  transition: 'transform 0.3s ease, box-shadow 0.3s ease',
  boxShadow: '0 4px 20px rgba(0, 0, 0, 0.1)',
  marginBottom: theme.spacing(2),
  '&:hover': {
    transform: 'translateY(-3px)',
    boxShadow: '0 8px 24px rgba(0, 0, 0, 0.15)',
  },
}));

const HashDisplay = styled(Box)(({ theme }) => ({
  display: 'flex',
  alignItems: 'center',
  padding: theme.spacing(0.5, 1),
  borderRadius: 4,
  backgroundColor: theme.palette.background.paper,
  maxWidth: '100%',
  overflow: 'hidden',
  whiteSpace: 'nowrap',
  textOverflow: 'ellipsis',
  fontFamily: 'monospace',
}));

const truncateHash = (hash, length = 10) => {
  if (!hash) return '';
  return `${hash.substring(0, length)}...${hash.substring(hash.length - length)}`;
};

// Function to copy text to clipboard
const copyToClipboard = (text) => {
  navigator.clipboard.writeText(text);
};

const formatTimestamp = (timestamp) => {
  if (!timestamp) return 'N/A';
  return new Date(timestamp * 1000).toLocaleString();
};

const BlockCard = ({ block }) => {
  const theme = useTheme();
  const [expanded, setExpanded] = useState(false);
  const { 
    blockNumber, 
    hash, 
    previousHash, 
    timestamp, 
    nonce, 
    difficulty,
    transactions = []
  } = block;
  
  const handleExpandClick = () => {
    setExpanded(!expanded);
  };
  
  return (
    <StyledCard>
      {/* Block Header */}
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
        <Box sx={{ display: 'flex', alignItems: 'center' }}>
          <Box 
            sx={{ 
              bgcolor: theme.palette.primary.main, 
              borderRadius: '50%', 
              p: 1, 
              mr: 2, 
              display: 'flex',
              boxShadow: '0 2px 10px rgba(0, 0, 0, 0.2)'
            }}
          >
            <CubeIcon sx={{ color: '#fff' }} />
          </Box>
          <Box>
            <Typography variant="body2" color="text.secondary">
              Block
            </Typography>
            <Typography variant="h6" fontWeight="bold">
              #{blockNumber}
            </Typography>
          </Box>
        </Box>
        
        <Box>
          <Typography variant="body2" color="text.secondary" textAlign="right" mb={0.5}>
            {formatTimestamp(timestamp)}
          </Typography>
          <Chip 
            label={`${transactions.length} Transaction${transactions.length !== 1 ? 's' : ''}`}
            size="small"
            color="info"
            variant="outlined"
          />
        </Box>
      </Box>
      
      {/* Block Hash */}
      <Box sx={{ mb: 2 }}>
        <Typography variant="body2" color="text.secondary" mb={0.5}>
          Block Hash
        </Typography>
        <HashDisplay>
          <Typography variant="body2" noWrap sx={{ flex: 1 }}>
            {hash}
          </Typography>
          <Tooltip title="Copy hash">
            <IconButton size="small" onClick={() => copyToClipboard(hash)}>
              <ContentCopyIcon fontSize="small" />
            </IconButton>
          </Tooltip>
        </HashDisplay>
      </Box>
      
      {/* Block Details */}
      <Box sx={{ mb: 1 }}>
        <Typography variant="body2" color="text.secondary" mb={0.5}>
          Previous Block Hash
        </Typography>
        <HashDisplay>
          <Typography variant="body2" noWrap sx={{ flex: 1 }}>
            {previousHash}
          </Typography>
          <Tooltip title="Copy hash">
            <IconButton size="small" onClick={() => copyToClipboard(previousHash)}>
              <ContentCopyIcon fontSize="small" />
            </IconButton>
          </Tooltip>
        </HashDisplay>
      </Box>
      
      {/* Nonce and Difficulty */}
      <Box sx={{ display: 'flex', mb: 2, mt: 2 }}>
        <Box sx={{ flex: 1, mr: 2 }}>
          <Typography variant="body2" color="text.secondary">
            Nonce
          </Typography>
          <Typography variant="body1" fontWeight="medium">
            {nonce}
          </Typography>
        </Box>
        <Box sx={{ flex: 1 }}>
          <Typography variant="body2" color="text.secondary">
            Difficulty
          </Typography>
          <Typography variant="body1" fontWeight="medium">
            {difficulty}
          </Typography>
        </Box>
      </Box>
      
      {/* Expand Button */}
      <Box 
        sx={{ 
          display: 'flex', 
          justifyContent: 'center', 
          mt: 1,
          cursor: 'pointer'
        }}
        onClick={handleExpandClick}
      >
        <IconButton
          aria-expanded={expanded}
          aria-label="show more"
        >
          {expanded ? <KeyboardArrowUpIcon /> : <KeyboardArrowDownIcon />}
        </IconButton>
        <Typography variant="body2" color="text.secondary" sx={{ mt: 1 }}>
          {expanded ? 'Hide' : 'Show'} transactions
        </Typography>
      </Box>
      
      {/* Transactions List */}
      <Collapse in={expanded} timeout="auto" unmountOnExit>
        <Divider sx={{ my: 2 }} />
        <Typography variant="subtitle2" gutterBottom>
          Transactions
        </Typography>
        <Box sx={{ mt: 1 }}>
          {transactions.length === 0 ? (
            <Typography variant="body2" color="text.secondary">
              No transactions in this block
            </Typography>
          ) : (
            <Stack spacing={1}>
              {transactions.map((tx, index) => (
                <Box 
                  key={tx.hash || index}
                  sx={{ 
                    p: 1.5, 
                    borderRadius: 1, 
                    bgcolor: theme.palette.background.paper,
                    border: `1px solid ${theme.palette.divider}`
                  }}
                >
                  <Typography variant="caption" color="text.secondary">
                    Tx Hash
                  </Typography>
                  <Box sx={{ display: 'flex', alignItems: 'center' }}>
                    <Typography variant="body2" fontFamily="monospace" noWrap sx={{ flex: 1 }}>
                      {truncateHash(tx.hash, 20)}
                    </Typography>
                    <Tooltip title="Copy hash">
                      <IconButton size="small" onClick={() => copyToClipboard(tx.hash)}>
                        <ContentCopyIcon fontSize="small" />
                      </IconButton>
                    </Tooltip>
                  </Box>
                  
                  <Box sx={{ display: 'flex', mt: 1 }}>
                    <Box sx={{ flex: 1 }}>
                      <Typography variant="caption" color="text.secondary">
                        From
                      </Typography>
                      <Typography variant="body2" fontFamily="monospace" noWrap>
                        {truncateHash(tx.sender, 8)}
                      </Typography>
                    </Box>
                    <Box sx={{ flex: 1 }}>
                      <Typography variant="caption" color="text.secondary">
                        To
                      </Typography>
                      <Typography variant="body2" fontFamily="monospace" noWrap>
                        {truncateHash(tx.receiver, 8)}
                      </Typography>
                    </Box>
                    <Box>
                      <Typography variant="caption" color="text.secondary">
                        Amount
                      </Typography>
                      <Typography variant="body2" fontWeight="medium" color="primary.main">
                        {tx.amount} $CLST
                      </Typography>
                    </Box>
                  </Box>
                </Box>
              ))}
            </Stack>
          )}
        </Box>
      </Collapse>
    </StyledCard>
  );
};

export default BlockCard; 