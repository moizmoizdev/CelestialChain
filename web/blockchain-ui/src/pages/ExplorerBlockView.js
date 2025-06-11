import React from 'react';
import { useParams } from 'react-router-dom';
import BlockDetail from '../components/Explorer/BlockDetail';
import { Box } from '@mui/material';

const ExplorerBlockView = () => {
  const { id } = useParams();
  
  return (
    <Box>
      <BlockDetail blockId={id} />
    </Box>
  );
};

export default ExplorerBlockView;