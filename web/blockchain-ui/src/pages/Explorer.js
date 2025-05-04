import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Grid, 
  TextField, 
  InputAdornment,
  IconButton,
  Button,
  Divider,
  Tab,
  Tabs,
  CircularProgress,
  List,
  ListItem,
  ListItemText,
  Card,
  CardContent,
  CardHeader,
  Chip,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow
} from '@mui/material';
import SearchIcon from '@mui/icons-material/Search';
import ExploreIcon from '@mui/icons-material/Explore';
import BlockIcon from '@mui/icons-material/ViewModule';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';
import AccountBoxIcon from '@mui/icons-material/AccountBox';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';

import { useNavigate } from 'react-router-dom';
import blockchainService from '../api/blockchainService';

const Explorer = () => {
  const navigate = useNavigate();
  const [loading, setLoading] = useState(false);
  const [searchInput, setSearchInput] = useState('');
  const [tabValue, setTabValue] = useState(0);
  const [blocks, setBlocks] = useState([]);
  const [transactions, setTransactions] = useState([]);
  const [statistics, setStatistics] = useState({
    blockCount: 0,
    transactionCount: 0,
    uniqueAddresses: 0,
    totalSupply: 0
  });

  useEffect(() => {
    fetchExplorerData();
  }, []);

  const fetchExplorerData = async () => {
    setLoading(true);
    try {
      // Get blockchain statistics
      const statsResponse = await blockchainService.getStatistics();
      setStatistics(statsResponse.data);

      // Get latest blocks
      const blocksResponse = await blockchainService.getLatestBlocks(5);
      if (blocksResponse.data) {
        const blocksData = Array.isArray(blocksResponse.data) 
          ? blocksResponse.data 
          : blocksResponse.data.blocks || [];
        setBlocks(blocksData);
      }

      // Get latest transactions
      const txResponse = await blockchainService.getLatestTransactions(10);
      if (txResponse.data) {
        const txData = Array.isArray(txResponse.data) 
          ? txResponse.data 
          : txResponse.data.transactions || [];
        setTransactions(txData);
      }
    } catch (error) {
      console.error('Error fetching explorer data:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleTabChange = (event, newValue) => {
    setTabValue(newValue);
  };

  const handleSearch = (e) => {
    e.preventDefault();
    if (!searchInput.trim()) return;

    const input = searchInput.trim();
    
    // Determine what type of data the user is searching for
    if (input.match(/^[0-9]+$/)) {
      // If input is only numbers, assume it's a block number
      navigate(`/explorer/block/${input}`);
    } else if (input.match(/^0x[a-fA-F0-9]{64}$/)) {
      // If input looks like a transaction hash (0x followed by 64 hex chars)
      navigate(`/explorer/transaction/${input}`);
    } else if (input.match(/^0x[a-fA-F0-9]{40}$/)) {
      // If input looks like an address (0x followed by 40 hex chars)
      navigate(`/explorer/address/${input}`);
    } else {
      // For any other input format, try the transaction route first
      navigate(`/explorer/transaction/${input}`);
    }
  };

  const truncateHash = (hash) => {
    if (!hash) return '';
    return hash.length > 16 
      ? `${hash.substring(0, 10)}...${hash.substring(hash.length - 6)}`
      : hash;
  };

  const formatTimestamp = (timestamp) => {
    if (!timestamp) return 'N/A';
    const date = new Date(timestamp * 1000);
    return date.toLocaleString();
  };

  const handleViewBlock = (blockNumber) => {
    navigate(`/explorer/block/${blockNumber}`);
  };

  const handleViewTransaction = (hash) => {
    navigate(`/explorer/transaction/${hash}`);
  };

  const handleViewAddress = (address) => {
    navigate(`/explorer/address/${address}`);
  };

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', alignItems: 'center' }}>
        <ExploreIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
        <Typography variant="h4" fontWeight="bold" gutterBottom>
          Blockchain Explorer
        </Typography>
      </Box>

      {/* Search Bar */}
      <Paper 
        component="form" 
        onSubmit={handleSearch}
        sx={{ 
          p: '2px 4px', 
          display: 'flex', 
          alignItems: 'center', 
          mb: 4,
          borderRadius: 2
        }}
      >
        <TextField
          fullWidth
          placeholder="Search by Block Number, Transaction Hash, or Address"
          value={searchInput}
          onChange={(e) => setSearchInput(e.target.value)}
          variant="outlined"
          InputProps={{
            startAdornment: (
              <InputAdornment position="start">
                <SearchIcon color="action" />
              </InputAdornment>
            ),
            endAdornment: (
              <InputAdornment position="end">
                <Button 
                  variant="contained" 
                  color="primary"
                  type="submit"
                  disabled={!searchInput.trim()}
                >
                  Search
                </Button>
              </InputAdornment>
            ),
          }}
          sx={{ ml: 1, flex: 1 }}
        />
      </Paper>

      {/* Blockchain Overview */}
      <Card sx={{ mb: 4, borderRadius: 2 }}>
        <CardHeader 
          title="Blockchain Overview" 
          sx={{ backgroundColor: 'primary.main', color: 'white' }}
        />
        <CardContent>
          <Grid container spacing={3}>
            <Grid item xs={6} md={3}>
              <Box textAlign="center">
                <Typography variant="h6" color="text.secondary">Blocks</Typography>
                <Typography variant="h4" fontWeight="bold">
                  {loading ? <CircularProgress size={24} /> : statistics.blockCount || statistics.blockHeight || 0}
                </Typography>
              </Box>
            </Grid>
            <Grid item xs={6} md={3}>
              <Box textAlign="center">
                <Typography variant="h6" color="text.secondary">Transactions</Typography>
                <Typography variant="h4" fontWeight="bold">
                  {loading ? <CircularProgress size={24} /> : statistics.transactionCount || 0}
                </Typography>
              </Box>
            </Grid>
            <Grid item xs={6} md={3}>
              <Box textAlign="center">
                <Typography variant="h6" color="text.secondary">Addresses</Typography>
                <Typography variant="h4" fontWeight="bold">
                  {loading ? <CircularProgress size={24} /> : statistics.uniqueAddresses || 0}
                </Typography>
              </Box>
            </Grid>
            <Grid item xs={6} md={3}>
              <Box textAlign="center">
                <Typography variant="h6" color="text.secondary">Total Supply</Typography>
                <Typography variant="h4" fontWeight="bold">
                  {loading ? <CircularProgress size={24} /> : `${statistics.totalSupply || 0} $CLST`}
                </Typography>
              </Box>
            </Grid>
          </Grid>
        </CardContent>
      </Card>

      {/* Tabs for Blocks and Transactions */}
      <Box sx={{ width: '100%', mb: 2 }}>
        <Tabs 
          value={tabValue} 
          onChange={handleTabChange} 
          variant="fullWidth"
          textColor="primary"
          indicatorColor="primary"
        >
          <Tab icon={<BlockIcon />} label="Latest Blocks" />
          <Tab icon={<SwapHorizIcon />} label="Latest Transactions" />
        </Tabs>
      </Box>

      {/* Tab Panels */}
      <div role="tabpanel" hidden={tabValue !== 0}>
        {tabValue === 0 && (
          <TableContainer component={Paper} sx={{ borderRadius: 2 }}>
            <Table>
              <TableHead sx={{ backgroundColor: 'grey.100' }}>
                <TableRow>
                  <TableCell width="15%">Block</TableCell>
                  <TableCell width="25%">Hash</TableCell>
                  <TableCell width="15%">Time</TableCell>
                  <TableCell width="15%">Transactions</TableCell>
                  <TableCell width="15%">Miner</TableCell>
                  <TableCell width="15%">Actions</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {loading ? (
                  <TableRow>
                    <TableCell colSpan={6} align="center">
                      <CircularProgress size={40} sx={{ my: 3 }} />
                    </TableCell>
                  </TableRow>
                ) : blocks.length > 0 ? (
                  blocks.map((block) => (
                    <TableRow key={block.blockNumber} hover>
                      <TableCell>{block.blockNumber}</TableCell>
                      <TableCell>{truncateHash(block.hash)}</TableCell>
                      <TableCell>{formatTimestamp(block.timestamp)}</TableCell>
                      <TableCell>
                        {block.transactions ? (
                          Array.isArray(block.transactions) 
                            ? block.transactions.length 
                            : '1'
                        ) : '0'}
                      </TableCell>
                      <TableCell>
                        {block.miner ? (
                          <Button 
                            size="small" 
                            variant="text"
                            onClick={() => handleViewAddress(block.miner)}
                          >
                            {truncateHash(block.miner)}
                          </Button>
                        ) : 'Genesis'}
                      </TableCell>
                      <TableCell>
                        <Button
                          variant="contained"
                          size="small"
                          endIcon={<ArrowForwardIcon />}
                          onClick={() => handleViewBlock(block.blockNumber)}
                        >
                          View
                        </Button>
                      </TableCell>
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={6} align="center">
                      No blocks found
                    </TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        )}
      </div>

      <div role="tabpanel" hidden={tabValue !== 1}>
        {tabValue === 1 && (
          <TableContainer component={Paper} sx={{ borderRadius: 2 }}>
            <Table>
              <TableHead sx={{ backgroundColor: 'grey.100' }}>
                <TableRow>
                  <TableCell width="30%">Transaction Hash</TableCell>
                  <TableCell width="15%">Block</TableCell>
                  <TableCell width="20%">From</TableCell>
                  <TableCell width="20%">To</TableCell>
                  <TableCell width="15%">Value</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {loading ? (
                  <TableRow>
                    <TableCell colSpan={5} align="center">
                      <CircularProgress size={40} sx={{ my: 3 }} />
                    </TableCell>
                  </TableRow>
                ) : transactions.length > 0 ? (
                  transactions.map((tx) => (
                    <TableRow key={tx.hash} hover>
                      <TableCell>
                        <Button 
                          size="small" 
                          variant="text"
                          onClick={() => handleViewTransaction(tx.hash)}
                        >
                          {truncateHash(tx.hash)}
                        </Button>
                      </TableCell>
                      <TableCell>
                        {tx.blockHeight || tx.blockNumber ? (
                          <Button 
                            size="small" 
                            variant="text"
                            onClick={() => handleViewBlock(tx.blockHeight || tx.blockNumber)}
                          >
                            {tx.blockHeight || tx.blockNumber}
                          </Button>
                        ) : (
                          <Chip label="Pending" size="small" color="warning" />
                        )}
                      </TableCell>
                      <TableCell>
                        <Button 
                          size="small" 
                          variant="text"
                          onClick={() => handleViewAddress(tx.sender)}
                        >
                          {truncateHash(tx.sender)}
                        </Button>
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
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={5} align="center">
                      No transactions found
                    </TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        )}
      </div>
    </Box>
  );
};

export default Explorer; 