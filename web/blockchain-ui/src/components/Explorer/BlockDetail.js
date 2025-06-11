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
  CircularProgress
} from '@mui/material';
import BlockIcon from '@mui/icons-material/ViewModule';
import { useNavigate } from 'react-router-dom';
import blockchainService from '../../api/blockchainService';

const BlockDetail = ({ blockId }) => {
  const navigate = useNavigate();
  const [loading, setLoading] = useState(true);
  const [block, setBlock] = useState(null);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchBlockData = async () => {
      setLoading(true);
      try {
        const response = await blockchainService.getBlockById(blockId);
        setBlock(response.data);
        setError(null);
      } catch (err) {
        console.error('Error fetching block:', err);
        setError('Failed to fetch block data. The block may not exist.');
      } finally {
        setLoading(false);
      }
    };

    if (blockId) {
      fetchBlockData();
    }
  }, [blockId]);

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

  const handleViewTransaction = (hash) => {
    navigate(`/explorer/transaction/${hash}`);
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
          title="Block Not Found" 
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

  if (!block) {
    return (
      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="No Block Data" 
          sx={{ backgroundColor: 'warning.main', color: 'white' }}
        />
        <CardContent>
          <Typography>No data available for this block.</Typography>
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
        <BlockIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
        <Typography variant="h4" fontWeight="bold">
          Block #{block.blockNumber}
        </Typography>
      </Box>

      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="Block Information" 
          sx={{ backgroundColor: 'primary.main', color: 'white' }}
        />
        <CardContent>
          <Grid container spacing={2}>
            <Grid item xs={12} md={3}>
              <Typography variant="subtitle2" color="text.secondary">Block Number</Typography>
              <Typography variant="body1" fontWeight="medium">{block.blockNumber}</Typography>
            </Grid>
            <Grid item xs={12} md={3}>
              <Typography variant="subtitle2" color="text.secondary">Timestamp</Typography>
              <Typography variant="body1">{formatTimestamp(block.timestamp)}</Typography>
            </Grid>
            <Grid item xs={12} md={3}>
              <Typography variant="subtitle2" color="text.secondary">Transactions</Typography>
              <Typography variant="body1">
                {block.transactions ? (
                  Array.isArray(block.transactions) 
                    ? block.transactions.length 
                    : '1'
                ) : '0'}
              </Typography>
            </Grid>
            <Grid item xs={12} md={3}>
              <Typography variant="subtitle2" color="text.secondary">Difficulty</Typography>
              <Typography variant="body1">{block.difficulty || 'N/A'}</Typography>
            </Grid>
            
            <Grid item xs={12}>
              <Divider sx={{ my: 1 }} />
            </Grid>
            
            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Hash</Typography>
              <Typography variant="body1" sx={{ wordBreak: 'break-all' }}>{block.hash}</Typography>
            </Grid>
            <Grid item xs={12} md={6}>
              <Typography variant="subtitle2" color="text.secondary">Previous Hash</Typography>
              <Typography variant="body1" sx={{ wordBreak: 'break-all' }}>{block.previousHash}</Typography>
            </Grid>
            
            <Grid item xs={12} md={3}>
              <Typography variant="subtitle2" color="text.secondary">Nonce</Typography>
              <Typography variant="body1">{block.nonce || 'N/A'}</Typography>
            </Grid>
            <Grid item xs={12} md={9}>
              <Typography variant="subtitle2" color="text.secondary">Miner</Typography>
              <Typography variant="body1">
                {block.miner ? (
                  <Button 
                    variant="text" 
                    color="primary"
                    onClick={() => handleViewAddress(block.miner)}
                  >
                    {block.miner}
                  </Button>
                ) : 'Genesis Block'}
              </Typography>
            </Grid>
          </Grid>
        </CardContent>
      </Card>

      {block.transactions && block.transactions.length > 0 && (
        <Card sx={{ borderRadius: 2 }}>
          <CardHeader 
            title="Transactions" 
            sx={{ backgroundColor: 'primary.main', color: 'white' }}
          />
          <CardContent>
            <TableContainer component={Paper} sx={{ borderRadius: 0 }}>
              <Table>
                <TableHead sx={{ backgroundColor: 'grey.100' }}>
                  <TableRow>
                    <TableCell width="30%">Hash</TableCell>
                    <TableCell width="25%">From</TableCell>
                    <TableCell width="25%">To</TableCell>
                    <TableCell width="10%">Value</TableCell>
                    <TableCell width="10%">Actions</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {Array.isArray(block.transactions) ? (
                    block.transactions.map((tx, index) => (
                      <TableRow key={tx.hash || index} hover>
                        <TableCell>
                          <Typography variant="body2" sx={{ wordBreak: 'break-all' }}>
                            {truncateHash(tx.hash)}
                          </Typography>
                        </TableCell>
                        <TableCell>
                          {tx.sender === 'Genesis' ? (
                            <Chip label="Mining Reward" size="small" color="success" />
                          ) : (
                            <Button 
                              size="small" 
                              variant="text"
                              onClick={() => handleViewAddress(tx.sender)}
                            >
                              {truncateHash(tx.sender)}
                            </Button>
                          )}
                        </TableCell>
                        <TableCell>
                          <Button 
                            size="small" 
                            variant="text"
                            onClick={() => handleViewAddress(tx.receiver)}
                          >
                            {truncateHash(tx.receiver)}
                          </Button>
                        </TableCell>
                        <TableCell>{tx.amount} $CLST</TableCell>
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
                    ))
                  ) : (
                    <TableRow>
                      <TableCell colSpan={5} align="center">
                        No transactions in this block
                      </TableCell>
                    </TableRow>
                  )}
                </TableBody>
              </Table>
            </TableContainer>
          </CardContent>
        </Card>
      )}
    </Box>
  );
};

export default BlockDetail; 