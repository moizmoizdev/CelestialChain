import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Skeleton,
  Divider
} from '@mui/material';
import blockchainService from '../api/blockchainService';

// Components
import BlockCard from '../components/UI/BlockCard';

// Simple timestamp formatter function
const formatTimestamp = (timestamp) => {
  if (!timestamp) return 'Unknown';
  
  const date = new Date(timestamp * 1000);
  return date.toLocaleString();
};

const Blockchain = () => {
  const [loading, setLoading] = useState(true);
  const [blocks, setBlocks] = useState([]);

  useEffect(() => {
    const fetchBlocks = async () => {
      try {
        setLoading(true);
        const response = await blockchainService.getAllBlocks(20); // Limit to 20 blocks for performance
        console.log('All blocks response:', response);
        
        if (response.data && Array.isArray(response.data)) {
          setBlocks(response.data);
        } else {
          console.error('Invalid blocks data:', response.data);
        }
      } catch (error) {
        console.error('Error fetching blocks:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchBlocks();
  }, []);

  return (
    <Box>
      <Box sx={{ mb: 4 }}>
        <Typography variant="h4" fontWeight="bold" gutterBottom>
          Blockchain
        </Typography>
        <Typography variant="body1" color="text.secondary">
          View and explore all blocks in the blockchain.
        </Typography>
      </Box>
      
      <Paper sx={{ p: 3, borderRadius: 2, mb: 4 }}>
        {loading ? (
          // Show skeletons while loading
          Array(5).fill(0).map((_, index) => (
            <Skeleton 
              key={index} 
              variant="rectangular" 
              height={160} 
              sx={{ mb: 2, borderRadius: 2 }} 
              animation="wave" 
            />
          ))
        ) : blocks.length > 0 ? (
          // Show blocks
          <Box>
            <Typography variant="h6" gutterBottom>
              All Blocks ({blocks.length})
            </Typography>
            <Divider sx={{ mb: 3 }} />
            
            {blocks.map((block) => (
              <BlockCard key={block.blockNumber} block={block} />
            ))}
          </Box>
        ) : (
          // No blocks found
          <Typography variant="body1">
            No blocks found. The blockchain may be empty or the API is not returning block data.
          </Typography>
        )}
      </Paper>
    </Box>
  );
};

export default Blockchain; 