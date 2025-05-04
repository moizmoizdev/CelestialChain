import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Button, 
  Grid, 
  Divider, 
  Slider, 
  Alert, 
  CircularProgress,
  Card,
  CardContent,
  IconButton,
  Tooltip,
  Chip
} from '@mui/material';
import MiningIcon from '@mui/icons-material/Hardware';
import RefreshIcon from '@mui/icons-material/Refresh';
import HelpOutlineIcon from '@mui/icons-material/HelpOutline';
import DifficultyIcon from '@mui/icons-material/Speed';
import RewardIcon from '@mui/icons-material/Payments';
import ScheduleIcon from '@mui/icons-material/Schedule';
import blockchainService from '../api/blockchainService';
import { useNavigate } from 'react-router-dom';

const Mining = () => {
  const navigate = useNavigate();
  const [stats, setStats] = useState({
    difficulty: 4,
    currentReward: 50,
    halvingsOccurred: 0,
    daysUntilNextHalving: 30,
    mempoolSize: 0,
    totalSupply: 0
  });
  
  const [loading, setLoading] = useState(false);
  const [miningLoading, setMiningLoading] = useState(false);
  const [difficultyLoading, setDifficultyLoading] = useState(false);
  const [newDifficulty, setNewDifficulty] = useState(4);
  const [error, setError] = useState(null);
  const [success, setSuccess] = useState(null);

  // Fetch mining stats on component mount
  useEffect(() => {
    fetchMiningStats();
    
    // Poll for updates every 10 seconds
    const interval = setInterval(() => {
      fetchMiningStats();
    }, 10000);
    
    return () => clearInterval(interval);
  }, []);

  const fetchMiningStats = async () => {
    try {
      setLoading(true);
      setError(null);
      
      // Get blockchain statistics
      const statsResponse = await blockchainService.getStatistics();
      console.log('Mining stats response:', statsResponse);
      
      if (statsResponse.data) {
        const data = statsResponse.data;
        
        setStats({
          difficulty: data.difficulty || 4,
          currentReward: data.currentReward || 50,
          halvingsOccurred: data.halving?.halvingsOccurred || 0,
          daysUntilNextHalving: data.halving?.daysUntilNextHalving || 30,
          mempoolSize: data.mempoolSize || data.mempool || 0,
          totalSupply: data.totalSupply || 0
        });
        
        // Update new difficulty slider with current difficulty
        setNewDifficulty(data.difficulty || 4);
      }
    } catch (error) {
      console.error('Error fetching mining stats:', error);
      setError('Failed to load mining statistics');
    } finally {
      setLoading(false);
    }
  };

  const calculateEstimatedTime = (difficulty) => {
    // Estimate mining time based on difficulty
    // Assuming average home computer can do about 10,000 hashes per second
    // Each difficulty level increases work by 16x (hexadecimal digit)
    const hashesNeeded = Math.pow(16, difficulty);
    const secondsNeeded = hashesNeeded / 10000;
    
    if (secondsNeeded < 60) {
      return `~${Math.ceil(secondsNeeded)} seconds`;
    } else if (secondsNeeded < 3600) {
      return `~${Math.ceil(secondsNeeded / 60)} minutes`;
    } else {
      return `~${(secondsNeeded / 3600).toFixed(1)} hours`;
    }
  };

  const handleDifficultyChange = (event, newValue) => {
    setNewDifficulty(newValue);
  };

  const handleUpdateDifficulty = async () => {
    try {
      setDifficultyLoading(true);
      setError(null);
      setSuccess(null);
      
      // Update difficulty through API
      const response = await blockchainService.setDifficulty(newDifficulty);
      console.log('Difficulty update response:', response);
      
      setSuccess(`Mining difficulty updated to ${newDifficulty}`);
      
      // Refresh mining stats
      fetchMiningStats();
    } catch (error) {
      console.error('Error updating difficulty:', error);
      setError('Failed to update mining difficulty');
    } finally {
      setDifficultyLoading(false);
    }
  };

  const handleMineBlock = async () => {
    try {
      setMiningLoading(true);
      setError(null);
      setSuccess(null);
      
      // Mine a new block through API
      const response = await blockchainService.mineBlock();
      console.log('Mining response:', response);
      
      if (response.data && response.data.hash) {
        setSuccess(`Block mined successfully! Hash: ${response.data.hash.substring(0, 15)}...`);
      } else {
        setSuccess('Block mined successfully!');
      }
      
      // Refresh mining stats
      fetchMiningStats();
    } catch (error) {
      console.error('Error mining block:', error);
      const errorMessage = error.response?.data?.error || 'Failed to mine block';
      setError(errorMessage);
    } finally {
      setMiningLoading(false);
    }
  };

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <Box>
          <Typography variant="h4" fontWeight="bold" gutterBottom>
            Mining
          </Typography>
          <Typography variant="body1" color="text.secondary">
            Mine new blocks and adjust mining difficulty.
          </Typography>
        </Box>
        
        <Button 
          variant="outlined" 
          startIcon={<RefreshIcon />} 
          onClick={fetchMiningStats}
          disabled={loading}
        >
          Refresh
        </Button>
      </Box>
      
      {error && (
        <Alert severity="error" sx={{ mb: 3 }} onClose={() => setError(null)}>
          {error}
        </Alert>
      )}
      
      {success && (
        <Alert severity="success" sx={{ mb: 3 }} onClose={() => setSuccess(null)}>
          {success}
        </Alert>
      )}
      
      <Grid container spacing={3}>
        {/* Mining Stats Card */}
        <Grid item xs={12} md={6}>
          <Paper sx={{ p: 3, borderRadius: 2, height: '100%' }}>
            <Typography variant="h6" gutterBottom>
              Mining Statistics
            </Typography>
            <Divider sx={{ mb: 2 }} />
            
            {loading ? (
              <Box sx={{ display: 'flex', justifyContent: 'center', py: 4 }}>
                <CircularProgress />
              </Box>
            ) : (
              <Grid container spacing={2}>
                <Grid item xs={12}>
                  <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                    <DifficultyIcon sx={{ mr: 1, color: 'primary.main' }} />
                    <Typography>
                      Current Difficulty: <strong>{stats.difficulty}</strong>
                    </Typography>
                    <Tooltip title="Higher difficulty requires more computational work to mine blocks">
                      <IconButton size="small" sx={{ ml: 1 }}>
                        <HelpOutlineIcon fontSize="small" />
                      </IconButton>
                    </Tooltip>
                  </Box>
                </Grid>
                
                <Grid item xs={12}>
                  <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                    <RewardIcon sx={{ mr: 1, color: 'primary.main' }} />
                    <Typography>
                      Current Mining Reward: <strong>{stats.currentReward} $CLST</strong>
                    </Typography>
                  </Box>
                </Grid>
                
                <Grid item xs={12}>
                  <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                    <ScheduleIcon sx={{ mr: 1, color: 'primary.main' }} />
                    <Typography>
                      Next Reward Halving: <strong>{stats.daysUntilNextHalving} days</strong> 
                      <Chip 
                        size="small" 
                        label={`${stats.halvingsOccurred} halvings so far`} 
                        sx={{ ml: 1 }} 
                        variant="outlined"
                      />
                    </Typography>
                  </Box>
                </Grid>
                
                <Grid item xs={12}>
                  <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                    <MiningIcon sx={{ mr: 1, color: 'primary.main' }} />
                    <Typography>
                      Total Supply: <strong>{stats.totalSupply} $CLST</strong>
                    </Typography>
                  </Box>
                </Grid>
                
                <Grid item xs={12}>
                  <Box sx={{ display: 'flex', alignItems: 'center' }}>
                    <Typography>
                      Mempool Transactions: <strong>{stats.mempoolSize}</strong>
                    </Typography>
                  </Box>
                </Grid>
              </Grid>
            )}
          </Paper>
        </Grid>
        
        {/* Mining Controls Card */}
        <Grid item xs={12} md={6}>
          <Paper sx={{ p: 3, borderRadius: 2, height: '100%' }}>
            <Typography variant="h6" gutterBottom>
              Mining Controls
            </Typography>
            <Divider sx={{ mb: 3 }} />
            
            <Box sx={{ mb: 4 }}>
              <Typography variant="subtitle1" gutterBottom>
                Mine a New Block
              </Typography>
              <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
                Mining a new block will include all pending transactions and generate a mining reward.
              </Typography>
              
              <Button
                variant="contained"
                startIcon={<MiningIcon />}
                onClick={handleMineBlock}
                disabled={miningLoading}
                fullWidth
                size="large"
              >
                {miningLoading ? (
                  <>
                    <CircularProgress size={24} sx={{ mr: 1 }} color="inherit" />
                    Mining...
                  </>
                ) : (
                  'Mine Block'
                )}
              </Button>
            </Box>
            
            <Box>
              <Typography variant="subtitle1" gutterBottom>
                Adjust Mining Difficulty
              </Typography>
              <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
                Current difficulty: {stats.difficulty} (Estimated mining time: {calculateEstimatedTime(stats.difficulty)})
              </Typography>
              
              <Box sx={{ px: 1 }}>
                <Slider
                  value={newDifficulty}
                  onChange={handleDifficultyChange}
                  step={1}
                  marks
                  min={1}
                  max={8}
                  valueLabelDisplay="auto"
                />
                
                <Box sx={{ display: 'flex', justifyContent: 'space-between', mb: 2 }}>
                  <Typography variant="caption" color="text.secondary">
                    Easier (1)
                  </Typography>
                  <Typography variant="caption" color="text.secondary">
                    Harder (8)
                  </Typography>
                </Box>
                
                <Typography variant="body2" sx={{ mb: 2 }}>
                  New difficulty: {newDifficulty} (Estimated mining time: {calculateEstimatedTime(newDifficulty)})
                </Typography>
              </Box>
              
              <Button
                variant="outlined"
                onClick={handleUpdateDifficulty}
                disabled={difficultyLoading || newDifficulty === stats.difficulty}
                fullWidth
              >
                {difficultyLoading ? (
                  <>
                    <CircularProgress size={24} sx={{ mr: 1 }} color="inherit" />
                    Updating...
                  </>
                ) : (
                  'Update Difficulty'
                )}
              </Button>
            </Box>
          </Paper>
        </Grid>
      </Grid>
    </Box>
  );
};

export default Mining; 