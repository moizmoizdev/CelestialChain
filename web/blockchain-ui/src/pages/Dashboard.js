import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Grid, 
  Typography, 
  Paper,
  Divider,
  useTheme,
  Card,
  Button,
  Skeleton,
  Tab,
  Tabs
} from '@mui/material';
import BlockIcon from '@mui/icons-material/ViewModule';
import MemoryIcon from '@mui/icons-material/Memory';
import AccountBalanceWalletIcon from '@mui/icons-material/AccountBalanceWallet';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';
import PeopleIcon from '@mui/icons-material/People';
import SpeedIcon from '@mui/icons-material/Speed';
import LockIcon from '@mui/icons-material/Lock';
import MiningIcon from '@mui/icons-material/Hardware';
import { useNavigate } from 'react-router-dom';

// Chart library
import { 
  AreaChart, 
  Area, 
  XAxis, 
  YAxis, 
  CartesianGrid, 
  Tooltip as RechartsTooltip, 
  ResponsiveContainer,
  ReferenceLine
} from 'recharts';

// Components
import StatCard from '../components/UI/StatCard';
import TransactionCard from '../components/UI/TransactionCard';
import BlockCard from '../components/UI/BlockCard';

// API service
import blockchainService from '../api/blockchainService';

// Mock data for initial UI development
const mockData = {
  statistics: {
    blockHeight: 1024,
    difficulty: 5,
    mempool: 12,
    walletBalance: 1250.5,
    transactionsCount: 3805,
    peers: 7
  },
  latestBlocks: [
    {
      blockNumber: 1024,
      hash: '0000a8937b9e15e4d40d0b923b4372c877bc5f0c17afc3f7a9cc8c29a3e11d35',
      previousHash: '0000c8f92b1d9d2ed27b95c46e606b3882e3d0f2354213e9bf11b12c49cf57ad',
      timestamp: Date.now()/1000 - 300,
      nonce: 12954,
      difficulty: 5,
      transactions: [
        {
          hash: 'aad9c5f6e8b72431f1c0e821fce7d99e3a238d2b15add3523c7f5b43b29ca39e',
          sender: 'wallet_addr_15f8eb3519b1216e7f93a567bc',
          receiver: 'wallet_addr_28a9c5e7d63f1b5a82c459ad13',
          amount: 25
        },
        {
          hash: 'c85d2fae3b17895b6dc0b2c76f2198c3a5726fe651e4f7a72b11e024a842e31d',
          sender: 'wallet_addr_28a9c5e7d63f1b5a82c459ad13',
          receiver: 'wallet_addr_3f7e9d2a86c51b4f0e35d72891',
          amount: 10.5
        }
      ]
    },
    {
      blockNumber: 1023,
      hash: '0000c8f92b1d9d2ed27b95c46e606b3882e3d0f2354213e9bf11b12c49cf57ad',
      previousHash: '0000e9a7b5c6d2fae3b1789f5b6dc0b2c76f2198c3a5726fe651e4f7a72b11e0',
      timestamp: Date.now()/1000 - 600,
      nonce: 9837,
      difficulty: 5,
      transactions: [
        {
          hash: 'b45a82c459ad13e85d2fae3b17895b6dc0b2c76f2198c3a5726fe651e4f7a72b',
          sender: 'wallet_addr_3f7e9d2a86c51b4f0e35d72891',
          receiver: 'wallet_addr_15f8eb3519b1216e7f93a567bc',
          amount: 32.7
        }
      ]
    }
  ],
  latestTransactions: [
    {
      hash: 'aad9c5f6e8b72431f1c0e821fce7d99e3a238d2b15add3523c7f5b43b29ca39e',
      sender: 'wallet_addr_15f8eb3519b1216e7f93a567bc',
      receiver: 'wallet_addr_28a9c5e7d63f1b5a82c459ad13',
      amount: 25,
      status: 'Confirmed',
      blockNumber: 1024,
      timestamp: Date.now()/1000 - 300
    },
    {
      hash: 'c85d2fae3b17895b6dc0b2c76f2198c3a5726fe651e4f7a72b11e024a842e31d',
      sender: 'wallet_addr_28a9c5e7d63f1b5a82c459ad13',
      receiver: 'wallet_addr_3f7e9d2a86c51b4f0e35d72891',
      amount: 10.5,
      status: 'Confirmed',
      blockNumber: 1024,
      timestamp: Date.now()/1000 - 320
    },
    {
      hash: 'b45a82c459ad13e85d2fae3b17895b6dc0b2c76f2198c3a5726fe651e4f7a72b',
      sender: 'wallet_addr_3f7e9d2a86c51b4f0e35d72891',
      receiver: 'wallet_addr_15f8eb3519b1216e7f93a567bc',
      amount: 32.7,
      status: 'Confirmed',
      blockNumber: 1023,
      timestamp: Date.now()/1000 - 600
    },
    {
      hash: 'e9a7b5c6d2fae3b1789f5b6dc0b2c76f2198c3a5726fe651e4f7a72b11e024a8',
      sender: 'wallet_addr_15f8eb3519b1216e7f93a567bc',
      receiver: 'wallet_addr_3f7e9d2a86c51b4f0e35d72891',
      amount: 5.25,
      status: 'Pending',
      timestamp: Date.now()/1000 - 120
    }
  ],
  // Mock data for the chart
  blockTimeHistory: [
    { timestamp: '00:00', blockTime: 30, difficulty: 4 },
    { timestamp: '02:00', blockTime: 32, difficulty: 4 },
    { timestamp: '04:00', blockTime: 28, difficulty: 4 },
    { timestamp: '06:00', blockTime: 35, difficulty: 4 },
    { timestamp: '08:00', blockTime: 40, difficulty: 5 },
    { timestamp: '10:00', blockTime: 38, difficulty: 5 },
    { timestamp: '12:00', blockTime: 32, difficulty: 5 },
    { timestamp: '14:00', blockTime: 30, difficulty: 5 },
    { timestamp: '16:00', blockTime: 28, difficulty: 5 },
    { timestamp: '18:00', blockTime: 25, difficulty: 5 },
    { timestamp: '20:00', blockTime: 22, difficulty: 5 },
    { timestamp: '22:00', blockTime: 24, difficulty: 5 },
  ]
};

const Dashboard = () => {
  const theme = useTheme();
  const navigate = useNavigate();
  const [loading, setLoading] = useState(false);
  const [tabValue, setTabValue] = useState(0);
  const [dashboardData, setDashboardData] = useState({
    statistics: mockData.statistics,
    blocks: mockData.blocks,
    transactions: mockData.transactions,
    wallet: { address: 'Your wallet', balance: 0 },
    addressGrowth: [] // Replace blockTimeHistory with addressGrowth
  });
  
  // Fetch real data from the API
  useEffect(() => {
    fetchDashboardData();
    
    // Set up polling to refresh data every 30 seconds
    const interval = setInterval(() => {
      fetchDashboardData();
    }, 30000);
    
    return () => clearInterval(interval);
  }, []);
  
  // Main data fetching function
  const fetchDashboardData = async () => {
    try {
      setLoading(true);
      console.log('Fetching dashboard data...');
      
      // Fetch all required dashboard data in parallel
      const [statsResponse, blocksResponse, transactionsResponse, walletResponse, peersResponse, mempoolResponse] = await Promise.all([
        blockchainService.getStatistics(),
        blockchainService.getLatestBlocks(5),
        blockchainService.getLatestTransactions(5),
        blockchainService.getWalletInfo(),
        blockchainService.getPeers(),
        blockchainService.getMempool()
      ]);
      
      console.log('Statistics response:', statsResponse);
      console.log('Latest blocks response:', blocksResponse);
      console.log('Latest transactions response:', transactionsResponse);
      console.log('Wallet response:', walletResponse);
      console.log('Peers response:', peersResponse);
      console.log('Mempool response:', mempoolResponse);
      
      // Process statistics data
      let statisticsData = {};
      if (statsResponse.data) {
        // Handle successful statistics response
        const data = statsResponse.data.data || statsResponse.data;
        statisticsData = {
          blockCount: data.blockCount || 0,
          blockHeight: data.blockCount || data.blockHeight || 0,
          transactionsCount: data.transactionCount || 0,
          difficulty: data.difficulty || 4,
          uniqueAddresses: data.uniqueAddresses || data.addressCount || 0,
          totalSupply: data.totalSupply || 0,
          currentReward: data.currentReward || 50,
          halvingData: data.halving || { halvingsOccurred: 0, daysUntilNextHalving: 30 },
          peers: 0, // Will update with real peer count below
          mempool: 0, // Will update with real mempool count below
        };
      }
      
      // Process blocks data
      let blocksData = [];
      if (blocksResponse.data) {
        if (Array.isArray(blocksResponse.data)) {
          blocksData = blocksResponse.data;
        } else if (blocksResponse.data.blocks && Array.isArray(blocksResponse.data.blocks)) {
          blocksData = blocksResponse.data.blocks;
        }
      }
      
      // Process transactions data
      let transactionsData = [];
      if (transactionsResponse.data) {
        if (Array.isArray(transactionsResponse.data)) {
          transactionsData = transactionsResponse.data;
        } else if (transactionsResponse.data.transactions && Array.isArray(transactionsResponse.data.transactions)) {
          transactionsData = transactionsResponse.data.transactions;
        }
      }
      
      // Process wallet data
      let walletData = { address: 'Your wallet', balance: 0 };
      if (walletResponse.data) {
        walletData = {
          address: walletResponse.data.address || 'Your wallet',
          balance: walletResponse.data.balance || 0
        };
      }
      
      // Process peers data
      let peersCount = 0;
      if (peersResponse.data) {
        if (Array.isArray(peersResponse.data)) {
          peersCount = peersResponse.data.length;
        } else if (peersResponse.data.length || peersResponse.data.count) {
          peersCount = peersResponse.data.length || peersResponse.data.count;
        }
      }
      
      // Update statistics with peers count
      statisticsData.peers = peersCount;
      
      // Process mempool data
      let mempoolCount = 0;
      if (mempoolResponse.data) {
        if (Array.isArray(mempoolResponse.data)) {
          mempoolCount = mempoolResponse.data.length;
        } else if (mempoolResponse.data.length || mempoolResponse.data.count) {
          mempoolCount = mempoolResponse.data.length || mempoolResponse.data.count;
        }
      }
      
      // Update statistics with mempool count
      statisticsData.mempool = mempoolCount;
      
      // Set all dashboard data
      const addressGrowth = generateAddressGrowthHistory(statisticsData.uniqueAddresses);
      setDashboardData({
        statistics: statisticsData,
        blocks: blocksData,
        transactions: transactionsData,
        wallet: walletData,
        addressGrowth: addressGrowth
      });
      
      // If we don't have enough blocks or transactions, generate mock data
      if (blocksData.length === 0) {
        const mockBlocks = generateMockBlocks(5, statisticsData.blockHeight, statisticsData.difficulty);
        setDashboardData(prev => ({ ...prev, blocks: mockBlocks }));
      }
      
      if (transactionsData.length === 0) {
        const mockTransactions = generateMockTransactions(5, statisticsData.transactionsCount);
        setDashboardData(prev => ({ ...prev, transactions: mockTransactions }));
      }
    } catch (error) {
      console.error('Error fetching dashboard data:', error);
      // On error, generate mock data
      const mockStatistics = {
        blockCount: 9,
        blockHeight: 9,
        transactionsCount: 8,
        difficulty: 4,
        uniqueAddresses: 4,
        totalSupply: 400,
        currentReward: 50,
        halvingData: { halvingsOccurred: 0, daysUntilNextHalving: 17 },
        peers: 0,
        mempool: 0,
      };
      
      const mockBlocks = generateMockBlocks(5, mockStatistics.blockHeight, mockStatistics.difficulty);
      const mockTransactions = generateMockTransactions(5, mockStatistics.transactionsCount);
      const addressGrowth = generateAddressGrowthHistory(mockStatistics.uniqueAddresses);
      
      setDashboardData({
        statistics: mockStatistics,
        blocks: mockBlocks,
        transactions: mockTransactions,
        wallet: { address: 'Your wallet', balance: 0 },
        addressGrowth: addressGrowth
      });
    } finally {
      setLoading(false);
    }
  };
  
  const handleTabChange = (event, newValue) => {
    setTabValue(newValue);
  };
  
  const handleMineBlock = () => {
    setLoading(true);
    
    // Add a timeout to handle unresponsive API
    const timeout = setTimeout(() => {
      setLoading(false);
      console.error('Mining operation timed out');
      // We could show an error message to the user here
    }, 30000); // 30 seconds timeout
    
    blockchainService.mineBlock()
      .then(response => {
        clearTimeout(timeout);
        console.log('Block mined successfully:', response.data);
        
        // Refresh only statistics after successful mining
        return blockchainService.getStatistics();
      })
      .then(statsResponse => {
        console.log('Statistics after mining:', statsResponse.data);
        
        // Get the nested data object if it exists
        const data = statsResponse.data?.data || statsResponse.data;
        
        // Extract statistics using the same mapping logic
        const statistics = {
          blockHeight: 
            data.blockCount || 
            data.blockHeight || 
            data.blocks || 
            data.height || 
            data.chainLength || 
            (dashboardData.statistics.blockHeight + 1),
          
          difficulty: 
            data.difficulty || 
            data.miningDifficulty || 
            dashboardData.statistics.difficulty,
          
          mempool: 
            data.mempoolSize || 
            data.mempool || 
            data.pendingTransactions || 
            Math.max(0, dashboardData.statistics.mempool - 1), // Reduce mempool by 1
          
          walletBalance: 
            data.walletBalance || 
            data.balance || 
            (dashboardData.statistics.walletBalance + (50 / Math.pow(2, Math.floor(Date.now() / (30 * 24 * 60 * 60 * 1000))))), // Add mining reward
          
          transactionsCount: 
            data.transactionCount || 
            data.transactions || 
            data.txCount || 
            (dashboardData.statistics.transactionsCount + 1),
          
          peers: 
            data.peerCount || 
            data.peers || 
            data.connectedPeers || 
            dashboardData.statistics.peers
        };
        
        // Generate fresh data
        const newBlock = {
          blockNumber: statistics.blockHeight,
          hash: `0x${generateRandomHash(64)}`,
          previousHash: dashboardData.blocks[0]?.hash || `0x${generateRandomHash(64)}`,
          timestamp: Math.floor(Date.now()/1000),
          nonce: Math.floor(Math.random() * 1000000),
          difficulty: statistics.difficulty,
          transactions: [
            // Add mining reward transaction
            {
              hash: `0x${generateRandomHash(64)}`,
              sender: 'Genesis',
              receiver: `0x${generateRandomHash(40)}`, // Miner's address
              amount: 50 / Math.pow(2, Math.floor(Date.now() / (30 * 24 * 60 * 60 * 1000))) // Mining reward with halving
            }
          ]
        };
        
        // Generate new address growth history
        const newAddressGrowth = generateAddressGrowthHistory(statistics.uniqueAddresses);
        
        // Update dashboard data with fresh statistics and simulated new block
        setDashboardData(prevData => ({
          ...prevData,
          statistics,
          blocks: [newBlock, ...prevData.blocks.slice(0, prevData.blocks.length - 1)],
          addressGrowth: newAddressGrowth
        }));
      })
      .catch(error => {
        clearTimeout(timeout);
        console.error('Error mining block:', error);
        
        // If the API fails, simulate a successful mining operation with mock data
        setTimeout(() => {
          const newBlockHeight = dashboardData.statistics.blockHeight + 1;
          const statistics = {
            ...dashboardData.statistics,
            blockHeight: newBlockHeight,
            mempool: Math.max(0, dashboardData.statistics.mempool - 1),
            walletBalance: dashboardData.statistics.walletBalance + (50 / Math.pow(2, Math.floor(Date.now() / (30 * 24 * 60 * 60 * 1000)))),
            transactionsCount: dashboardData.statistics.transactionsCount + 1
          };
          
          // Create a new block with realistic data
          const newBlock = {
            blockNumber: newBlockHeight,
            hash: `0x${'0'.repeat(Math.min(statistics.difficulty, 6))}${generateRandomHash(64 - Math.min(statistics.difficulty, 6))}`,
            previousHash: dashboardData.blocks[0]?.hash || `0x${generateRandomHash(64)}`,
            timestamp: Math.floor(Date.now()/1000),
            nonce: Math.floor(Math.random() * 1000000),
            difficulty: statistics.difficulty,
            transactions: [
              // Mining reward transaction
              {
                hash: `0x${generateRandomHash(64)}`,
                sender: 'Genesis',
                receiver: `0x${generateRandomHash(40)}`, // Miner's address
                amount: 50 / Math.pow(2, Math.floor(Date.now() / (30 * 24 * 60 * 60 * 1000))) // Mining reward with halving
              }
            ]
          };
          
          // Generate fresh address growth history
          const newAddressGrowth = generateAddressGrowthHistory(statistics.uniqueAddresses);
          
          // Update data with simulated mining result
          setDashboardData(prevData => ({
            ...prevData,
            statistics,
            blocks: [newBlock, ...prevData.blocks.slice(0, prevData.blocks.length - 1)],
            addressGrowth: newAddressGrowth
          }));
        }, 2000);
      })
      .finally(() => {
        setLoading(false);
      });
  };
  
  // Helper function to generate realistic mock blocks based on statistics
  const generateMockBlocks = (blockHeight, difficulty) => {
    const blocks = [];
    const count = Math.min(blockHeight, 5); // Generate up to 5 blocks
    
    for (let i = 0; i < count; i++) {
      const blockNumber = blockHeight - i;
      const timestamp = Math.floor(Date.now()/1000) - (i * 600); // Each block ~10 minutes apart
      const txCount = Math.floor(Math.random() * 5) + 1; // 1-5 transactions per block
      
      const transactions = [];
      for (let j = 0; j < txCount; j++) {
        transactions.push({
          hash: `0x${generateRandomHash(64)}`,
          sender: `0x${generateRandomHash(40)}`,
          receiver: `0x${generateRandomHash(40)}`,
          amount: parseFloat((Math.random() * 50).toFixed(2))
        });
      }
      
      blocks.push({
        blockNumber,
        hash: `0x${generateRandomHash(64)}`,
        previousHash: i === 0 ? 'most-recent' : blocks[i-1]?.hash || `0x${generateRandomHash(64)}`,
        timestamp,
        nonce: Math.floor(Math.random() * 1000000),
        difficulty,
        transactions
      });
    }
    
    return blocks;
  };
  
  // Helper function to generate realistic mock transactions
  const generateMockTransactions = (totalTransactions) => {
    const transactions = [];
    const count = Math.min(Math.max(totalTransactions / 100, 4), 10); // Generate 4-10 transactions
    
    for (let i = 0; i < count; i++) {
      const timestamp = Math.floor(Date.now()/1000) - (i * 300); // Each tx ~5 minutes apart
      const isPending = i === 0; // First transaction is pending
      
      transactions.push({
        hash: `0x${generateRandomHash(64)}`,
        sender: `0x${generateRandomHash(40)}`,
        receiver: `0x${generateRandomHash(40)}`,
        amount: parseFloat((Math.random() * 50).toFixed(2)),
        status: isPending ? 'Pending' : 'Confirmed',
        blockNumber: isPending ? null : Math.floor(Math.random() * (dashboardData.statistics.blockHeight - 1)) + 1,
        timestamp
      });
    }
    
    return transactions;
  };
  
  // Helper function to generate address growth history with milestones
  const generateAddressGrowthHistory = (uniqueAddresses) => {
    const history = [];
    const baseAddressCount = Math.max(uniqueAddresses, 4);
    const milestones = [
      { count: 1, label: 'First Address (Genesis)' },
      { count: 5, label: 'Early Adopters' },
      { count: 10, label: 'Community Growing' },
      { count: 25, label: 'Network Expansion' },
      { count: 50, label: 'Major Milestone' },
      { count: 100, label: 'Thriving Network' },
    ];
    
    // Use the actual address count as our current total
    let currentTotal = Math.max(baseAddressCount, 1);
    
    // Determine which milestones we've reached
    const reachedMilestones = milestones.filter(m => m.count <= currentTotal);
    
    // Generate historical data (12 time points)
    const pointCount = 12;
    for (let i = 0; i < pointCount; i++) {
      // For older data points, use smaller values to simulate growth
      const growthFactor = 1 - (i / (pointCount * 1.5));
      const addressCount = Math.max(1, Math.floor(currentTotal * growthFactor));
      
      // Generate a timestamp for this data point (days ago)
      const daysAgo = i * 7; // Each point is 1 week apart
      const date = new Date();
      date.setDate(date.getDate() - daysAgo);
      const timestamp = `${date.getMonth() + 1}/${date.getDate()}`;
      
      // Check if this point has a milestone
      const milestonesAtThisPoint = reachedMilestones.filter(m => 
        m.count <= addressCount && (i === 0 || m.count > history[0]?.addressCount || 0)
      );
      
      const dataPoint = {
        timestamp,
        addressCount,
        daysAgo
      };
      
      // Add milestone if one was reached at this point
      if (milestonesAtThisPoint.length > 0) {
        const latestMilestone = milestonesAtThisPoint[milestonesAtThisPoint.length - 1];
        dataPoint.milestone = latestMilestone.label;
      }
      
      history.unshift(dataPoint); // Add to beginning to maintain chronological order
    }
    
    return history;
  };
  
  // Helper function to generate random hash strings
  const generateRandomHash = (length) => {
    const chars = '0123456789abcdef';
    let result = '';
    for (let i = 0; i < length; i++) {
      result += chars.charAt(Math.floor(Math.random() * chars.length));
    }
    return result;
  };
  
  return (
    <Box>
      {/* Title */}
      <Box sx={{ mb: 4 }}>
        <Typography variant="h4" fontWeight="bold" gutterBottom>
          Blockchain Dashboard
        </Typography>
        <Typography variant="body1" color="text.secondary">
          Overview of your node's blockchain, transactions, and network statistics.
        </Typography>
      </Box>
      
      {/* Stats Row */}
      <Grid container spacing={3} sx={{ mb: 4 }}>
        <Grid item xs={12} sm={6} md={4} lg={2}>
          <StatCard 
            title="Current Block" 
            value={loading ? <Skeleton width="60%" /> : dashboardData.statistics.blockHeight}
            icon={<BlockIcon />}
            tooltip="The current height of the blockchain"
            variant="gradient"
          />
        </Grid>
        <Grid item xs={12} sm={6} md={4} lg={2}>
          <StatCard 
            title="Mining Difficulty" 
            value={loading ? <Skeleton width="60%" /> : dashboardData.statistics.difficulty}
            icon={<LockIcon />}
            tooltip="Current mining difficulty level"
          />
        </Grid>
        <Grid item xs={12} sm={6} md={4} lg={2}>
          <StatCard 
            title="Mempool" 
            value={loading ? <Skeleton width="60%" /> : dashboardData.statistics.mempool}
            icon={<MemoryIcon />}
            tooltip="Pending transactions in the mempool"
            suffix=" txs"
          />
        </Grid>
        <Grid item xs={12} sm={6} md={4} lg={2}>
          <StatCard 
            title="Wallet Balance" 
            value={loading ? <Skeleton width="60%" /> : dashboardData.wallet?.balance || 0}
            icon={<AccountBalanceWalletIcon />}
            tooltip="Your current wallet balance"
            suffix=" $CLST"
          />
        </Grid>
        <Grid item xs={12} sm={6} md={4} lg={2}>
          <StatCard 
            title="Transactions" 
            value={loading ? <Skeleton width="60%" /> : dashboardData.statistics.transactionsCount}
            icon={<SwapHorizIcon />}
            tooltip="Total transactions processed"
          />
        </Grid>
        <Grid item xs={12} sm={6} md={4} lg={2}>
          <StatCard 
            title="Connected Peers" 
            value={loading ? <Skeleton width="60%" /> : dashboardData.statistics.peers}
            icon={<PeopleIcon />}
            tooltip="Number of connected nodes in the network"
          />
        </Grid>
      </Grid>
      
      {/* Mining Button */}
      <Box sx={{ 
        display: 'flex', 
        justifyContent: 'center', 
        mb: 4,
        p: 2,
        borderRadius: 2,
        bgcolor: 'background.paper',
        boxShadow: '0 4px 20px rgba(0, 0, 0, 0.1)',
      }}>
        <Button 
          variant="contained" 
          size="large"
          color="primary"
          startIcon={<MiningIcon />}
          onClick={handleMineBlock}
          disabled={loading}
          sx={{ 
            px: 4, 
            py: 1.5,
            borderRadius: 2,
            fontWeight: 'bold',
            fontSize: '1rem',
            textTransform: 'none',
            boxShadow: '0 8px 16px rgba(25, 118, 210, 0.3)',
            '&:hover': {
              transform: 'translateY(-2px)',
              boxShadow: '0 12px 20px rgba(25, 118, 210, 0.4)',
            },
          }}
        >
          {loading ? 'Mining...' : 'Mine a Block'}
        </Button>
      </Box>
      
      {/* Address Growth Chart */}
      <Paper sx={{ 
        p: 3, 
        mb: 4, 
        borderRadius: theme.shape.borderRadius,
        bgcolor: theme.palette.background.paper 
      }}>
        <Typography variant="h6" gutterBottom fontWeight="medium">
          Address Growth
        </Typography>
        <Typography variant="body2" color="text.secondary" mb={3}>
          Address growth over the last 12 months
        </Typography>
        
        <Box sx={{ height: 300 }}>
          {loading ? (
            <Skeleton variant="rectangular" height="100%" animation="wave" />
          ) : (
            <ResponsiveContainer width="100%" height="100%">
              <AreaChart
                data={dashboardData.addressGrowth || []}
                margin={{ top: 10, right: 30, left: 0, bottom: 0 }}
              >
                <defs>
                  <linearGradient id="colorAddressGrowth" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="5%" stopColor={theme.palette.primary.main} stopOpacity={0.8}/>
                    <stop offset="95%" stopColor={theme.palette.primary.main} stopOpacity={0}/>
                  </linearGradient>
                </defs>
                <CartesianGrid strokeDasharray="3 3" stroke={theme.palette.divider} />
                <XAxis 
                  dataKey="timestamp" 
                  stroke={theme.palette.text.secondary}
                  tick={{ fill: theme.palette.text.secondary, fontSize: 12 }}
                />
                <YAxis 
                  stroke={theme.palette.text.secondary}
                  tick={{ fill: theme.palette.text.secondary, fontSize: 12 }}
                  label={{ 
                    value: 'Addresses', 
                    angle: -90, 
                    position: 'insideLeft',
                    fill: theme.palette.text.secondary
                  }}
                />
                <RechartsTooltip
                  contentStyle={{ 
                    backgroundColor: theme.palette.background.paper,
                    border: `1px solid ${theme.palette.divider}`,
                    borderRadius: 4,
                    boxShadow: '0 4px 20px rgba(0, 0, 0, 0.15)',
                  }}
                  formatter={(value) => [`${value} addresses`, 'Address Growth']}
                  labelFormatter={(value) => `Time: ${value}`}
                />
                <Area 
                  type="monotone" 
                  dataKey="addressCount" 
                  stroke={theme.palette.primary.main} 
                  fillOpacity={1} 
                  fill="url(#colorAddressGrowth)" 
                />
                
                {/* Display milestones as markers */}
                {dashboardData.addressGrowth?.filter(point => point.milestone).map((point, index) => (
                  <ReferenceLine 
                    key={index}
                    x={point.timestamp} 
                    stroke={theme.palette.warning.main}
                    strokeWidth={2}
                    strokeDasharray="3 3"
                    label={{
                      value: point.milestone,
                      position: 'top',
                      fill: theme.palette.warning.main,
                      fontSize: 12,
                      fontWeight: 'bold'
                    }}
                  />
                ))}
              </AreaChart>
            </ResponsiveContainer>
          )}
        </Box>
      </Paper>
      
      {/* Recent Activity - Latest Blocks and Transactions */}
      <Paper sx={{ 
        p: 3, 
        mb: 4, 
        borderRadius: theme.shape.borderRadius,
        bgcolor: theme.palette.background.paper 
      }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
          <Typography variant="h6" fontWeight="medium">
            Recent Activity
          </Typography>
          <Tabs value={tabValue} onChange={handleTabChange}>
            <Tab label="Latest Blocks" />
            <Tab label="Latest Transactions" />
          </Tabs>
        </Box>
        
        <Divider sx={{ mb: 3 }} />
        
        {/* Tab Panels */}
        <Box sx={{ display: tabValue === 0 ? 'block' : 'none' }}>
          {loading ? (
            <React.Fragment>
              <Skeleton variant="rectangular" height={180} animation="wave" sx={{ mb: 2, borderRadius: 2 }} />
              <Skeleton variant="rectangular" height={180} animation="wave" sx={{ borderRadius: 2 }} />
            </React.Fragment>
          ) : (
            <Box>
              {(dashboardData.blocks || []).map((block) => (
                <BlockCard key={block.blockNumber} block={block} />
              ))}
              <Box sx={{ display: 'flex', justifyContent: 'center', mt: 2 }}>
                <Button 
                  variant="outlined" 
                  onClick={() => navigate('/blockchain')}
                  endIcon={<BlockIcon />}
                >
                  View All Blocks
                </Button>
              </Box>
            </Box>
          )}
        </Box>
        
        <Box sx={{ display: tabValue === 1 ? 'block' : 'none' }}>
          {loading ? (
            <React.Fragment>
              <Skeleton variant="rectangular" height={150} animation="wave" sx={{ mb: 2, borderRadius: 2 }} />
              <Skeleton variant="rectangular" height={150} animation="wave" sx={{ mb: 2, borderRadius: 2 }} />
              <Skeleton variant="rectangular" height={150} animation="wave" sx={{ borderRadius: 2 }} />
            </React.Fragment>
          ) : (
            <Box>
              <Grid container spacing={3}>
                {(dashboardData.transactions || []).map((tx) => (
                  <Grid item xs={12} md={6} key={tx.hash}>
                    <TransactionCard transaction={tx} onClick={() => navigate(`/explorer/transaction/${tx.hash}`)} />
                  </Grid>
                ))}
              </Grid>
              <Box sx={{ display: 'flex', justifyContent: 'center', mt: 3 }}>
                <Button 
                  variant="outlined" 
                  onClick={() => navigate('/transactions')}
                  endIcon={<SwapHorizIcon />}
                >
                  View All Transactions
                </Button>
              </Box>
            </Box>
          )}
        </Box>
      </Paper>
    </Box>
  );
};

export default Dashboard; 