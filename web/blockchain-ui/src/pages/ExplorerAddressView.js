import React from 'react';
import { useParams } from 'react-router-dom';
import AddressDetail from '../components/Explorer/AddressDetail';
import { Box } from '@mui/material';

const ExplorerAddressView = () => {
  const { address } = useParams();
  
  return (
    <Box>
      <AddressDetail address={address} />
    </Box>
  );
};

export default ExplorerAddressView;