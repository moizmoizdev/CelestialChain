import React from 'react';
import { Box, Card, Typography, IconButton, Tooltip } from '@mui/material';
import { styled } from '@mui/material/styles';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';

const StyledCard = styled(Card)(({ theme, variant }) => ({
  padding: theme.spacing(3),
  borderRadius: theme.shape.borderRadius,
  transition: 'transform 0.3s ease, box-shadow 0.3s ease',
  boxShadow: '0 4px 20px rgba(0, 0, 0, 0.15)',
  height: '100%',
  display: 'flex',
  flexDirection: 'column',
  position: 'relative',
  background: variant === 'gradient' ? 
    `linear-gradient(45deg, ${theme.palette.primary.dark} 0%, ${theme.palette.primary.main} 100%)` : 
    theme.palette.background.card,
  '&:hover': {
    transform: 'translateY(-5px)',
    boxShadow: '0 8px 25px rgba(0, 0, 0, 0.2)',
  },
}));

const IconWrapper = styled(Box)(({ theme, variant }) => ({
  borderRadius: '50%',
  display: 'flex',
  alignItems: 'center',
  justifyContent: 'center',
  padding: theme.spacing(1.2),
  marginBottom: theme.spacing(1.5),
  width: 48,
  height: 48,
  background: variant === 'gradient' ? 
    'rgba(255, 255, 255, 0.2)' : 
    theme.palette.primary.main,
  color: variant === 'gradient' ? 
    theme.palette.common.white : 
    theme.palette.common.white,
}));

const StatCard = ({ 
  title, 
  value, 
  icon, 
  tooltip, 
  variant = 'default', 
  trend, 
  trendValue,
  suffix = ''
}) => {
  return (
    <StyledCard variant={variant}>
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', mb: 1 }}>
        <Box>
          <IconWrapper variant={variant}>
            {icon}
          </IconWrapper>
          <Typography 
            variant="body2" 
            sx={{ 
              mb: 0.5, 
              fontWeight: 500,
              color: variant === 'gradient' ? 'rgba(255, 255, 255, 0.8)' : 'text.secondary'
            }}
          >
            {title}
          </Typography>
          <Typography 
            variant="h4" 
            sx={{ 
              fontWeight: 700,
              color: variant === 'gradient' ? '#fff' : 'text.primary' 
            }}
          >
            {value}{suffix}
          </Typography>
          
          {trend && (
            <Box 
              sx={{ 
                display: 'flex', 
                alignItems: 'center', 
                mt: 1, 
                color: trend === 'up' ? 'success.main' : 'error.main',
                bgcolor: trend === 'up' ? 'rgba(76, 175, 80, 0.1)' : 'rgba(244, 67, 54, 0.1)',
                px: 1,
                py: 0.5,
                borderRadius: 1,
                width: 'fit-content'
              }}
            >
              {trend === 'up' ? '↑' : '↓'} {trendValue}
            </Box>
          )}
        </Box>
        
        {tooltip && (
          <Tooltip title={tooltip} placement="top">
            <IconButton size="small" sx={{ 
              color: variant === 'gradient' ? 'rgba(255, 255, 255, 0.7)' : 'text.secondary'
            }}>
              <InfoOutlinedIcon fontSize="small" />
            </IconButton>
          </Tooltip>
        )}
      </Box>
    </StyledCard>
  );
};

export default StatCard; 