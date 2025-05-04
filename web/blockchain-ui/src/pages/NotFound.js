import React from 'react';
import { Box, Typography, Button, Paper } from '@mui/material';
import { useNavigate } from 'react-router-dom';
import HomeIcon from '@mui/icons-material/Home';
import ErrorOutlineIcon from '@mui/icons-material/ErrorOutline';

const NotFound = () => {
  const navigate = useNavigate();

  return (
    <Box
      sx={{
        display: 'flex',
        justifyContent: 'center',
        alignItems: 'center',
        minHeight: 'calc(100vh - 200px)',
      }}
    >
      <Paper
        elevation={3}
        sx={{
          p: 5,
          maxWidth: 500,
          textAlign: 'center',
          borderRadius: 3,
          boxShadow: '0 10px 30px rgba(0, 0, 0, 0.2)',
          background: 'linear-gradient(135deg, #1E1E1E 0%, #252525 100%)',
        }}
      >
        <ErrorOutlineIcon 
          sx={{ 
            fontSize: 80, 
            color: 'error.main',
            mb: 2
          }} 
        />
        <Typography variant="h3" component="h1" fontWeight="bold" gutterBottom>
          404
        </Typography>
        <Typography variant="h5" component="h2" gutterBottom>
          Page Not Found
        </Typography>
        <Typography variant="body1" color="text.secondary" paragraph>
          The page you are looking for doesn't exist or has been moved.
        </Typography>
        <Button
          variant="contained"
          startIcon={<HomeIcon />}
          size="large"
          onClick={() => navigate('/')}
          sx={{ 
            mt: 2,
            px: 4,
            py: 1,
            borderRadius: 2,
            fontWeight: 'bold',
            background: 'linear-gradient(45deg, #1976D2 30%, #2196F3 90%)',
            boxShadow: '0 5px 15px rgba(33, 150, 243, 0.3)',
            '&:hover': {
              boxShadow: '0 8px 20px rgba(33, 150, 243, 0.4)',
              transform: 'translateY(-2px)'
            }
          }}
        >
          Back to Home
        </Button>
      </Paper>
    </Box>
  );
};

export default NotFound; 