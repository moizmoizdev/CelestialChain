import React from 'react';
import { useParams } from 'react-router-dom';
import TransactionDetail from '../components/Explorer/TransactionDetail';
import { Box } from '@mui/material';

const ExplorerTransactionView = () => {
  const { hash } = useParams();
  
  return (
    <Box>
      <TransactionDetail hash={hash} />
    </Box>
  );
};

export default ExplorerTransactionView;