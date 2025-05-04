import React, { useState } from 'react';
import { 
  Box, 
  Drawer, 
  AppBar, 
  Toolbar, 
  List, 
  Typography, 
  Divider, 
  IconButton, 
  ListItem, 
  ListItemButton, 
  ListItemIcon, 
  ListItemText,
  useTheme,
  Tooltip,
  Avatar,
  Chip
} from '@mui/material';
import { styled } from '@mui/material/styles';
import { useNavigate, useLocation } from 'react-router-dom';

// Components
import NodeSelector from '../NodeSelector';
import NodeStatus from '../NodeStatus';

// Icons
import MenuIcon from '@mui/icons-material/Menu';
import ChevronLeftIcon from '@mui/icons-material/ChevronLeft';
import DashboardIcon from '@mui/icons-material/Dashboard';
import BlockIcon from '@mui/icons-material/ViewModule';
import MemoryIcon from '@mui/icons-material/Memory';
import AccountBalanceWalletIcon from '@mui/icons-material/AccountBalanceWallet';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';
import PeopleIcon from '@mui/icons-material/People';
import SettingsIcon from '@mui/icons-material/Settings';
import ExploreIcon from '@mui/icons-material/Explore';
import SyncIcon from '@mui/icons-material/Sync';
import MiningIcon from '@mui/icons-material/Hardware';

const drawerWidth = 260;

const openedMixin = (theme) => ({
  width: drawerWidth,
  transition: theme.transitions.create('width', {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.enteringScreen,
  }),
  overflowX: 'hidden',
  backgroundColor: theme.palette.background.paper,
  borderRight: `1px solid ${theme.palette.divider}`,
});

const closedMixin = (theme) => ({
  transition: theme.transitions.create('width', {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.leavingScreen,
  }),
  overflowX: 'hidden',
  backgroundColor: theme.palette.background.paper,
  borderRight: `1px solid ${theme.palette.divider}`,
  width: `calc(${theme.spacing(7)} + 1px)`,
  [theme.breakpoints.up('sm')]: {
    width: `calc(${theme.spacing(8)} + 1px)`,
  },
});

const StyledDrawer = styled(Drawer, { shouldForwardProp: (prop) => prop !== 'open' })(
  ({ theme, open }) => ({
    width: drawerWidth,
    flexShrink: 0,
    whiteSpace: 'nowrap',
    boxSizing: 'border-box',
    ...(open && {
      ...openedMixin(theme),
      '& .MuiDrawer-paper': openedMixin(theme),
    }),
    ...(!open && {
      ...closedMixin(theme),
      '& .MuiDrawer-paper': closedMixin(theme),
    }),
  }),
);

const MainContent = styled('main', { shouldForwardProp: (prop) => prop !== 'open' })(
  ({ theme, open }) => ({
    flexGrow: 1,
    padding: theme.spacing(3),
    transition: theme.transitions.create('margin', {
      easing: theme.transitions.easing.sharp,
      duration: theme.transitions.duration.leavingScreen,
    }),
    marginLeft: 0,
    ...(open && {
      transition: theme.transitions.create('margin', {
        easing: theme.transitions.easing.easeOut,
        duration: theme.transitions.duration.enteringScreen,
      }),
      marginLeft: 0,
    }),
  }),
);

const DrawerHeader = styled('div')(({ theme }) => ({
  display: 'flex',
  alignItems: 'center',
  justifyContent: 'space-between',
  padding: theme.spacing(0, 1),
  // necessary for content to be below app bar
  ...theme.mixins.toolbar,
}));

const MainLayout = ({ children }) => {
  const theme = useTheme();
  const navigate = useNavigate();
  const location = useLocation();
  const [open, setOpen] = useState(true);

  const handleDrawerOpen = () => {
    setOpen(true);
  };

  const handleDrawerClose = () => {
    setOpen(false);
  };

  const menuItems = [
    { label: 'Dashboard', icon: <DashboardIcon />, path: '/' },
    { label: 'Blockchain', icon: <BlockIcon />, path: '/blockchain' },
    { label: 'Mempool', icon: <MemoryIcon />, path: '/mempool' },
    { label: 'Mine Blocks', icon: <MiningIcon />, path: '/mine' },
    { label: 'Transactions', icon: <SwapHorizIcon />, path: '/transactions' },
    { label: 'Wallet', icon: <AccountBalanceWalletIcon />, path: '/wallet' },
    { label: 'Explorer', icon: <ExploreIcon />, path: '/explorer' },
    { label: 'Peers', icon: <PeopleIcon />, path: '/peers' },
    { label: 'Sync Network', icon: <SyncIcon />, path: '/sync' },
    { label: 'Settings', icon: <SettingsIcon />, path: '/settings' },
  ];

  const isActive = (path) => {
    return location.pathname === path;
  };

  return (
    <Box sx={{ display: 'flex', height: '100vh', overflow: 'hidden' }}>
      <AppBar position="fixed" sx={{ zIndex: theme.zIndex.drawer + 1 }}>
        <Toolbar>
          <IconButton
            color="inherit"
            aria-label="open drawer"
            onClick={handleDrawerOpen}
            edge="start"
            sx={{ mr: 2, ...(open && { display: 'none' }) }}
          >
            <MenuIcon />
          </IconButton>
          
          <Box sx={{ display: 'flex', alignItems: 'center' }}>
            <BlockIcon sx={{ fontSize: 28, mr: 1.5 }} />
            <Typography variant="h5" noWrap component="div" sx={{ fontWeight: 'bold' }}>
              CelestialChain
            </Typography>
          </Box>
          
          <Box sx={{ flexGrow: 1 }} />
          
          <NodeStatus sx={{ mr: 2 }} />
          
          <NodeSelector />
          
          <Chip
            color="success"
            variant="outlined"
            label="Connected"
            size="small"
            sx={{ mr: 2 }}
          />
          
          <Tooltip title="Wallet">
            <IconButton color="inherit" onClick={() => navigate('/wallet')}>
              <Avatar sx={{ width: 32, height: 32, bgcolor: theme.palette.secondary.main }}>
                <AccountBalanceWalletIcon fontSize="small" />
              </Avatar>
            </IconButton>
          </Tooltip>
        </Toolbar>
      </AppBar>
      
      <StyledDrawer variant="permanent" open={open}>
        <DrawerHeader>
          <Box sx={{ 
            display: 'flex', 
            alignItems: 'center', 
            justifyContent: 'center',
            mx: 'auto'
          }}>
            {open && (
              <>
                <BlockIcon sx={{ mr: 1, color: theme.palette.primary.main }} />
                <Typography variant="h6" color="primary" sx={{ fontWeight: 'bold' }}>
                  CelestialChain
                </Typography>
              </>
            )}
          </Box>
          <IconButton onClick={handleDrawerClose}>
            <ChevronLeftIcon />
          </IconButton>
        </DrawerHeader>
        
        <Divider />
        
        <List>
          {menuItems.map((item) => (
            <ListItem key={item.label} disablePadding sx={{ display: 'block' }}>
              <ListItemButton
                sx={{
                  minHeight: 48,
                  justifyContent: open ? 'initial' : 'center',
                  px: 2.5,
                  backgroundColor: isActive(item.path) ? 'rgba(25, 118, 210, 0.12)' : 'transparent',
                  borderLeft: isActive(item.path) ? `4px solid ${theme.palette.primary.main}` : '4px solid transparent',
                  '&:hover': {
                    backgroundColor: 'rgba(25, 118, 210, 0.08)',
                  },
                }}
                onClick={() => navigate(item.path)}
              >
                <ListItemIcon
                  sx={{
                    minWidth: 0,
                    mr: open ? 3 : 'auto',
                    justifyContent: 'center',
                    color: isActive(item.path) ? theme.palette.primary.main : theme.palette.text.secondary,
                  }}
                >
                  {item.icon}
                </ListItemIcon>
                <ListItemText 
                  primary={item.label} 
                  sx={{ 
                    opacity: open ? 1 : 0,
                    '& .MuiTypography-root': {
                      fontWeight: isActive(item.path) ? 600 : 400,
                      color: isActive(item.path) ? theme.palette.primary.main : theme.palette.text.primary,
                    }
                  }} 
                />
              </ListItemButton>
            </ListItem>
          ))}
        </List>
      </StyledDrawer>
      
      <Box sx={{ flexGrow: 1, p: 3, overflow: 'auto', height: '100vh' }}>
        <DrawerHeader />
        {children}
      </Box>
    </Box>
  );
};

export default MainLayout; 