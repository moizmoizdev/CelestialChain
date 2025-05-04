import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Paper, 
  Slider, 
  Grid, 
  TextField,
  Button,
  Alert,
  Snackbar,
  Divider,
  Card,
  CardContent,
  CardActions,
  FormControlLabel,
  Switch,
  CircularProgress,
  Select,
  MenuItem,
  FormControl,
  InputLabel
} from '@mui/material';
import SettingsIcon from '@mui/icons-material/Settings';
import SaveIcon from '@mui/icons-material/Save';
import BuildIcon from '@mui/icons-material/Build';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import blockchainService from '../api/blockchainService';

const difficultyMarks = [
  { value: 1, label: '1' },
  { value: 2, label: '2' },
  { value: 3, label: '3' },
  { value: 4, label: '4' },
  { value: 5, label: '5' },
  { value: 6, label: '6' },
  { value: 7, label: '7' },
  { value: 8, label: '8' },
];

const durationFormatter = (seconds) => {
  if (seconds < 60) {
    return `${Math.round(seconds)} seconds`;
  } else if (seconds < 3600) {
    return `${Math.round(seconds / 60)} minutes`;
  } else {
    return `${(seconds / 3600).toFixed(1)} hours`;
  }
};

const Settings = () => {
  const [settings, setSettings] = useState({
    difficulty: 4,
    nodeType: 'full',
    autoMine: false,
    miningInterval: 60,
    apiPort: 8080
  });

  const [estimatedMiningTime, setEstimatedMiningTime] = useState(0);
  const [loading, setLoading] = useState(false);
  const [saving, setSaving] = useState(false);
  const [snackbar, setSnackbar] = useState({
    open: false,
    message: '',
    severity: 'info'
  });

  useEffect(() => {
    fetchSettings();
  }, []);

  useEffect(() => {
    // Calculate estimated mining time based on difficulty
    // Formula: 16^difficulty / 10000 operations per second
    const timeInSeconds = Math.pow(16, settings.difficulty) / 10000;
    setEstimatedMiningTime(timeInSeconds);
  }, [settings.difficulty]);

  const fetchSettings = async () => {
    setLoading(true);
    try {
      const response = await blockchainService.getSettings();
      if (response.data) {
        setSettings(prevSettings => ({
          ...prevSettings,
          ...response.data
        }));
      }
    } catch (error) {
      console.error('Error fetching settings:', error);
      setSnackbar({
        open: true,
        message: 'Failed to load settings',
        severity: 'error'
      });
    } finally {
      setLoading(false);
    }
  };

  const handleSaveSettings = async () => {
    setSaving(true);
    try {
      const response = await blockchainService.updateSettings(settings);
      if (response.data && response.data.success) {
        setSnackbar({
          open: true,
          message: 'Settings saved successfully',
          severity: 'success'
        });
      } else {
        setSnackbar({
          open: true,
          message: 'Failed to save settings',
          severity: 'error'
        });
      }
    } catch (error) {
      console.error('Error saving settings:', error);
      setSnackbar({
        open: true,
        message: 'Error saving settings',
        severity: 'error'
      });
    } finally {
      setSaving(false);
    }
  };

  const handleDifficultyChange = (event, newValue) => {
    setSettings({ ...settings, difficulty: newValue });
  };

  const handleInputChange = (event) => {
    const { name, value } = event.target;
    setSettings({ ...settings, [name]: value });
  };

  const handleSwitchChange = (event) => {
    const { name, checked } = event.target;
    setSettings({ ...settings, [name]: checked });
  };

  const handleResetSettings = () => {
    setSettings({
      difficulty: 4,
      nodeType: 'full',
      autoMine: false,
      miningInterval: 60,
      apiPort: 8080
    });
    setSnackbar({
      open: true,
      message: 'Settings reset to default',
      severity: 'info'
    });
  };

  const handleCloseSnackbar = () => {
    setSnackbar({ ...snackbar, open: false });
  };

  if (loading) {
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '70vh' }}>
        <CircularProgress />
      </Box>
    );
  }

  return (
    <Box>
      <Box sx={{ mb: 4, display: 'flex', alignItems: 'center' }}>
        <SettingsIcon sx={{ mr: 2, fontSize: 36, color: 'primary.main' }} />
        <Typography variant="h4" fontWeight="bold" gutterBottom>
          Blockchain Settings
        </Typography>
      </Box>

      <Grid container spacing={3}>
        <Grid item xs={12} md={6}>
          <Card sx={{ mb: 3 }}>
            <CardContent>
              <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                <BuildIcon sx={{ mr: 1, color: 'primary.main' }} />
                <Typography variant="h6">Mining Configuration</Typography>
              </Box>
              <Divider sx={{ mb: 3 }} />

              <Typography gutterBottom>Mining Difficulty: {settings.difficulty}</Typography>
              <Slider
                value={settings.difficulty}
                onChange={handleDifficultyChange}
                step={1}
                marks={difficultyMarks}
                min={1}
                max={8}
                valueLabelDisplay="auto"
                sx={{ mb: 3 }}
              />

              <Alert severity="info" sx={{ mb: 2 }}>
                Estimated mining time: {durationFormatter(estimatedMiningTime)}
              </Alert>

              <FormControlLabel
                control={
                  <Switch
                    checked={settings.autoMine}
                    onChange={handleSwitchChange}
                    name="autoMine"
                  />
                }
                label="Auto-mine blocks"
                sx={{ mb: 2 }}
              />

              <TextField
                label="Mining Interval (seconds)"
                type="number"
                name="miningInterval"
                value={settings.miningInterval}
                onChange={handleInputChange}
                fullWidth
                variant="outlined"
                disabled={!settings.autoMine}
                sx={{ mb: 2 }}
                InputProps={{ inputProps: { min: 5, max: 3600 } }}
              />
            </CardContent>
          </Card>

          <Card>
            <CardContent>
              <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
                <SettingsIcon sx={{ mr: 1, color: 'primary.main' }} />
                <Typography variant="h6">Node Configuration</Typography>
              </Box>
              <Divider sx={{ mb: 3 }} />

              <FormControl fullWidth sx={{ mb: 3 }}>
                <InputLabel>Node Type</InputLabel>
                <Select
                  value={settings.nodeType}
                  label="Node Type"
                  name="nodeType"
                  onChange={handleInputChange}
                >
                  <MenuItem value="full">Full Node</MenuItem>
                  <MenuItem value="wallet">Wallet Node</MenuItem>
                </Select>
              </FormControl>

              <TextField
                label="API Port"
                type="number"
                name="apiPort"
                value={settings.apiPort}
                onChange={handleInputChange}
                fullWidth
                variant="outlined"
                sx={{ mb: 2 }}
                InputProps={{ inputProps: { min: 1024, max: 65535 } }}
              />
            </CardContent>
          </Card>
        </Grid>

        <Grid item xs={12} md={6}>
          <Card sx={{ mb: 3 }}>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Blockchain Information
              </Typography>
              <Divider sx={{ mb: 3 }} />
              
              <Typography variant="body1" sx={{ mb: 1 }}>
                Genesis Timestamp: 1745026508
              </Typography>
              
              <Typography variant="body1" sx={{ mb: 1 }}>
                Genesis Block Hash: 0x0000eb99d08f42f3c322b891f18212c85aa05365166964973a56d03e7da36f80
              </Typography>
              
              <Typography variant="body1" sx={{ mb: 1 }}>
                Initial Mining Reward: 50 $CLST
              </Typography>
              
              <Typography variant="body1" sx={{ mb: 1 }}>
                Halving Interval: 30 days
              </Typography>
            </CardContent>
          </Card>

          <Card>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Advanced Settings
              </Typography>
              <Divider sx={{ mb: 3 }} />
              
              <Alert severity="warning" sx={{ mb: 3 }}>
                These actions can affect blockchain operations. Use with caution.
              </Alert>

              <Button
                variant="outlined"
                color="warning"
                fullWidth
                startIcon={<RestartAltIcon />}
                onClick={handleResetSettings}
                sx={{ mb: 2 }}
              >
                Reset to Default Settings
              </Button>

              <Button
                variant="outlined"
                color="error"
                fullWidth
                startIcon={<RestartAltIcon />}
                sx={{ mb: 2 }}
              >
                Restart Node
              </Button>
            </CardContent>
          </Card>
        </Grid>
      </Grid>

      <Box sx={{ mt: 3, display: 'flex', justifyContent: 'flex-end' }}>
        <Button
          variant="contained"
          color="primary"
          startIcon={<SaveIcon />}
          onClick={handleSaveSettings}
          disabled={saving}
          sx={{ minWidth: 150 }}
        >
          {saving ? <CircularProgress size={24} /> : 'Save Settings'}
        </Button>
      </Box>

      <Snackbar
        open={snackbar.open}
        autoHideDuration={6000}
        onClose={handleCloseSnackbar}
        anchorOrigin={{ vertical: 'bottom', horizontal: 'right' }}
      >
        <Alert
          onClose={handleCloseSnackbar}
          severity={snackbar.severity}
          variant="filled"
          sx={{ width: '100%' }}
        >
          {snackbar.message}
        </Alert>
      </Snackbar>
    </Box>
  );
};

export default Settings; 