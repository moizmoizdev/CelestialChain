import { createTheme } from '@mui/material/styles';

// Create a blockchain-themed dark color scheme
const theme = createTheme({
  palette: {
    mode: 'dark',
    primary: {
      main: '#1976D2', // Blue
      light: '#4791db',
      dark: '#115293',
      contrastText: '#fff',
    },
    secondary: {
      main: '#7B1FA2', // Purple
      light: '#9c27b0',
      dark: '#6a1b9a',
      contrastText: '#fff',
    },
    success: {
      main: '#4caf50', // Green for success/confirmed transactions
      light: '#81c784',
      dark: '#388e3c',
    },
    warning: {
      main: '#ff9800', // Orange for pending transactions
      light: '#ffb74d',
      dark: '#f57c00',
    },
    error: {
      main: '#f44336', // Red
      light: '#e57373',
      dark: '#d32f2f',
    },
    info: {
      main: '#2196f3', // Light blue
      light: '#64b5f6',
      dark: '#1976d2',
    },
    background: {
      default: '#121212',
      paper: '#1E1E1E',
      card: '#252525',
      elevated: '#2c2c2c',
    },
    text: {
      primary: '#FFFFFF',
      secondary: 'rgba(255, 255, 255, 0.7)',
      disabled: 'rgba(255, 255, 255, 0.5)',
      hint: 'rgba(255, 255, 255, 0.3)',
    },
    divider: 'rgba(255, 255, 255, 0.12)',
  },
  typography: {
    fontFamily: [
      'Inter',
      'Roboto',
      '-apple-system',
      'BlinkMacSystemFont',
      '"Segoe UI"',
      'Arial',
      'sans-serif',
    ].join(','),
    h1: {
      fontSize: '2.5rem',
      fontWeight: 700,
    },
    h2: {
      fontSize: '2rem',
      fontWeight: 600,
    },
    h3: {
      fontSize: '1.75rem',
      fontWeight: 600,
    },
    h4: {
      fontSize: '1.5rem',
      fontWeight: 600,
    },
    h5: {
      fontSize: '1.25rem',
      fontWeight: 600,
    },
    h6: {
      fontSize: '1rem',
      fontWeight: 600,
    },
    subtitle1: {
      fontSize: '1rem',
      fontWeight: 400,
    },
    subtitle2: {
      fontSize: '0.875rem',
      fontWeight: 500,
    },
    body1: {
      fontSize: '1rem',
    },
    body2: {
      fontSize: '0.875rem',
    },
    button: {
      textTransform: 'none', // Avoid uppercase text in buttons
      fontWeight: 500,
    },
  },
  shape: {
    borderRadius: 12,
  },
  components: {
    MuiButton: {
      styleOverrides: {
        root: {
          borderRadius: 8,
          padding: '10px 20px',
          transition: 'all 0.2s ease-in-out',
          '&:hover': {
            transform: 'translateY(-2px)',
            boxShadow: '0 5px 10px rgba(0, 0, 0, 0.2)',
          },
        },
        containedPrimary: {
          background: 'linear-gradient(45deg, #1976D2 30%, #2196F3 90%)',
        },
        containedSecondary: {
          background: 'linear-gradient(45deg, #7B1FA2 30%, #9C27B0 90%)',
        },
      },
    },
    MuiCard: {
      styleOverrides: {
        root: {
          backgroundColor: '#252525',
          boxShadow: '0 4px 20px rgba(0, 0, 0, 0.25)',
          borderRadius: 16,
          transition: 'transform 0.3s ease, box-shadow 0.3s ease',
          '&:hover': {
            transform: 'translateY(-5px)',
            boxShadow: '0 10px 30px rgba(0, 0, 0, 0.3)',
          },
        },
      },
    },
    MuiPaper: {
      styleOverrides: {
        root: {
          backgroundImage: 'none',
        },
        elevation1: {
          boxShadow: '0 2px 10px rgba(0, 0, 0, 0.2)',
        },
        elevation2: {
          boxShadow: '0 4px 15px rgba(0, 0, 0, 0.2)',
        },
      },
    },
    MuiAppBar: {
      styleOverrides: {
        root: {
          boxShadow: '0 4px 20px rgba(0, 0, 0, 0.15)',
          backdropFilter: 'blur(10px)',
          backgroundColor: 'rgba(18, 18, 18, 0.8)',
        },
      },
    },
    MuiTableCell: {
      styleOverrides: {
        root: {
          borderBottom: '1px solid rgba(255, 255, 255, 0.1)',
        },
        head: {
          fontWeight: 600,
          backgroundColor: '#1E1E1E',
        },
      },
    },
    MuiChip: {
      styleOverrides: {
        root: {
          borderRadius: 8,
        },
      },
    },
  },
});

export default theme; 