import React, { useState, useEffect } from 'react';
import { 
  Box, 
  Button, 
  Typography, 
  Menu, 
  MenuItem,
  IconButton,
  Tooltip,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  List,
  ListItem,
  ListItemText,
  ListItemSecondaryAction,
  CircularProgress,
  Snackbar,
  Alert
} from '@mui/material';
import LanguageIcon from '@mui/icons-material/Language';
import EditIcon from '@mui/icons-material/Edit';
import DeleteIcon from '@mui/icons-material/Delete';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import ErrorIcon from '@mui/icons-material/Error';
import AddIcon from '@mui/icons-material/Add';
import SettingsIcon from '@mui/icons-material/Settings';
import blockchainService from '../api/blockchainService';

const NodeSelector = () => {
  const nodeManager = blockchainService.getNodeManager();
  
  // State for nodes and UI
  const [nodes, setNodes] = useState([]);
  const [activeNode, setActiveNode] = useState(null);
  const [menuAnchor, setMenuAnchor] = useState(null);
  const [manageDialogOpen, setManageDialogOpen] = useState(false);
  const [editDialogOpen, setEditDialogOpen] = useState(false);
  const [editNodeIndex, setEditNodeIndex] = useState(-1);
  const [nodeName, setNodeName] = useState('');
  const [nodeUrl, setNodeUrl] = useState('');
  const [testingIndex, setTestingIndex] = useState(-1);
  const [snackbar, setSnackbar] = useState({ open: false, message: '', severity: 'info' });

  // Load nodes on component mount
  useEffect(() => {
    loadNodes();
    
    // Add listener for node changes
    const handleNodeChange = () => {
      loadNodes();
    };
    
    nodeManager.addListener(handleNodeChange);
    
    // Clean up listener on unmount
    return () => {
      nodeManager.removeListener(handleNodeChange);
    };
  }, []);

  // Load nodes from manager
  const loadNodes = () => {
    const allNodes = nodeManager.getNodes();
    setNodes(allNodes);
    setActiveNode(nodeManager.getActiveNode());
  };

  // Open node selection menu
  const handleOpenMenu = (event) => {
    setMenuAnchor(event.currentTarget);
  };

  // Close node selection menu
  const handleCloseMenu = () => {
    setMenuAnchor(null);
  };

  // Select a node
  const handleSelectNode = (index) => {
    nodeManager.setActiveNode(index);
    loadNodes();
    handleCloseMenu();
    setSnackbar({
      open: true,
      message: `Connected to ${nodes[index].name}`,
      severity: 'success'
    });
  };

  // Open management dialog
  const handleOpenManage = () => {
    handleCloseMenu();
    setManageDialogOpen(true);
  };

  // Close management dialog
  const handleCloseManage = () => {
    setManageDialogOpen(false);
  };

  // Open dialog to add new node
  const handleOpenAddNode = () => {
    setEditNodeIndex(-1);
    setNodeName('');
    setNodeUrl('');
    setEditDialogOpen(true);
  };

  // Open dialog to edit existing node
  const handleOpenEditNode = (index) => {
    setEditNodeIndex(index);
    setNodeName(nodes[index].name);
    setNodeUrl(nodes[index].url);
    setEditDialogOpen(true);
  };

  // Close add/edit dialog
  const handleCloseEditDialog = () => {
    setEditDialogOpen(false);
  };

  // Save node changes
  const handleSaveNode = () => {
    if (!nodeName || !nodeUrl) {
      setSnackbar({
        open: true,
        message: 'Name and URL are required',
        severity: 'error'
      });
      return;
    }

    if (editNodeIndex === -1) {
      // Add new node
      const success = nodeManager.addNode(nodeName, nodeUrl);
      if (success) {
        setSnackbar({
          open: true,
          message: 'Node added successfully',
          severity: 'success'
        });
      } else {
        setSnackbar({
          open: true,
          message: 'Failed to add node. URL might already exist.',
          severity: 'error'
        });
      }
    } else {
      // Edit existing node
      const success = nodeManager.editNode(editNodeIndex, nodeName, nodeUrl);
      if (success) {
        setSnackbar({
          open: true,
          message: 'Node updated successfully',
          severity: 'success'
        });
      } else {
        setSnackbar({
          open: true,
          message: 'Failed to update node',
          severity: 'error'
        });
      }
    }

    loadNodes();
    setEditDialogOpen(false);
  };

  // Delete a node
  const handleDeleteNode = (index) => {
    if (nodes.length <= 1) {
      setSnackbar({
        open: true,
        message: 'Cannot delete the only node',
        severity: 'warning'
      });
      return;
    }

    const success = nodeManager.removeNode(index);
    if (success) {
      setSnackbar({
        open: true,
        message: 'Node removed successfully',
        severity: 'success'
      });
    } else {
      setSnackbar({
        open: true,
        message: 'Failed to remove node',
        severity: 'error'
      });
    }
    
    loadNodes();
  };

  // Test connection to a node
  const handleTestConnection = async (index) => {
    setTestingIndex(index);
    try {
      const success = await nodeManager.testConnection(nodes[index].url);
      setSnackbar({
        open: true,
        message: success ? 'Connection successful' : 'Connection failed',
        severity: success ? 'success' : 'error'
      });
    } catch (error) {
      setSnackbar({
        open: true,
        message: 'Connection test failed',
        severity: 'error'
      });
    }
    setTestingIndex(-1);
  };

  // Reset to default nodes
  const handleResetNodes = () => {
    nodeManager.resetToDefaults();
    loadNodes();
    setSnackbar({
      open: true,
      message: 'Reset to default nodes',
      severity: 'info'
    });
  };

  // Close snackbar
  const handleCloseSnackbar = () => {
    setSnackbar({ ...snackbar, open: false });
  };

  // Extract port number from URL
  const getPortFromUrl = (url) => {
    const match = url.match(/:(\d+)/);
    return match ? match[1] : 'unknown';
  };

  return (
    <>
      <Tooltip title="Select Blockchain Node">
        <Button
          variant="text"
          color="inherit"
          startIcon={<LanguageIcon />}
          onClick={handleOpenMenu}
          sx={{ textTransform: 'none' }}
        >
          {activeNode?.name || 'Select Node'} {activeNode && `(Port: ${getPortFromUrl(activeNode.url)})`}
        </Button>
      </Tooltip>

      {/* Node Selection Menu */}
      <Menu
        anchorEl={menuAnchor}
        open={Boolean(menuAnchor)}
        onClose={handleCloseMenu}
      >
        {nodes.map((node, index) => (
          <MenuItem
            key={index}
            onClick={() => handleSelectNode(index)}
            selected={node.isActive}
          >
            {node.name}
            <Typography variant="caption" color="text.secondary" sx={{ ml: 1 }}>
              ({getPortFromUrl(node.url)})
            </Typography>
            {node.isActive && <CheckCircleIcon color="success" fontSize="small" sx={{ ml: 1 }} />}
          </MenuItem>
        ))}
        <MenuItem divider />
        <MenuItem onClick={handleOpenManage}>
          <SettingsIcon fontSize="small" sx={{ mr: 1 }} />
          Manage Nodes
        </MenuItem>
      </Menu>

      {/* Node Management Dialog */}
      <Dialog
        open={manageDialogOpen}
        onClose={handleCloseManage}
        maxWidth="sm"
        fullWidth
      >
        <DialogTitle>Manage Blockchain Nodes</DialogTitle>
        <DialogContent>
          <List>
            {nodes.map((node, index) => (
              <ListItem key={index} divider>
                <ListItemText 
                  primary={
                    <Box component="span" sx={{ display: 'flex', alignItems: 'center' }}>
                      {node.name}
                      <Typography variant="caption" color="text.secondary" sx={{ ml: 1 }}>
                        (Port: {getPortFromUrl(node.url)})
                      </Typography>
                    </Box>
                  }
                  secondary={node.url}
                  primaryTypographyProps={{
                    fontWeight: node.isActive ? 'bold' : 'normal'
                  }}
                />
                <ListItemSecondaryAction>
                  {testingIndex === index ? (
                    <CircularProgress size={24} />
                  ) : (
                    <>
                      <IconButton
                        edge="end"
                        onClick={() => handleTestConnection(index)}
                        title="Test Connection"
                      >
                        {node.isActive ? <CheckCircleIcon color="success" /> : <LanguageIcon />}
                      </IconButton>
                      <IconButton
                        edge="end"
                        onClick={() => handleOpenEditNode(index)}
                        title="Edit Node"
                      >
                        <EditIcon />
                      </IconButton>
                      <IconButton
                        edge="end"
                        onClick={() => handleDeleteNode(index)}
                        title="Delete Node"
                        disabled={nodes.length <= 1}
                      >
                        <DeleteIcon />
                      </IconButton>
                    </>
                  )}
                </ListItemSecondaryAction>
              </ListItem>
            ))}
          </List>
          <Box sx={{ mt: 2, display: 'flex', justifyContent: 'center' }}>
            <Button
              startIcon={<AddIcon />}
              variant="contained"
              onClick={handleOpenAddNode}
              sx={{ mr: 1 }}
            >
              Add Node
            </Button>
            <Button
              color="secondary"
              onClick={handleResetNodes}
            >
              Reset to Defaults
            </Button>
          </Box>
          
          {/* Add a more visible Reset Configuration alert/button for users experiencing issues */}
          <Box sx={{ mt: 3, p: 2, border: '1px dashed', borderColor: 'warning.main', borderRadius: 1, bgcolor: 'warning.light' }}>
            <Typography variant="subtitle2" color="warning.dark" sx={{ mb: 1 }}>
              Seeing incorrect node configurations or connection issues?
            </Typography>
            <Button
              color="warning"
              variant="contained"
              onClick={handleResetNodes}
              fullWidth
              sx={{ mt: 1 }}
            >
              Reset Node Configuration
            </Button>
            <Typography variant="caption" color="text.secondary" sx={{ mt: 1, display: 'block' }}>
              This will restore the default node configuration with the correct API ports (8080, 8081, 8085, 8090).
            </Typography>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCloseManage}>Close</Button>
        </DialogActions>
      </Dialog>

      {/* Add/Edit Node Dialog */}
      <Dialog
        open={editDialogOpen}
        onClose={handleCloseEditDialog}
        maxWidth="sm"
        fullWidth
      >
        <DialogTitle>{editNodeIndex === -1 ? 'Add New Node' : 'Edit Node'}</DialogTitle>
        <DialogContent>
          <TextField
            label="Node Name"
            fullWidth
            margin="normal"
            value={nodeName}
            onChange={(e) => setNodeName(e.target.value)}
          />
          <TextField
            label="Node URL"
            fullWidth
            margin="normal"
            value={nodeUrl}
            onChange={(e) => setNodeUrl(e.target.value)}
            placeholder="http://localhost:8080/api"
            helperText="Include the full URL with protocol and /api path"
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCloseEditDialog}>Cancel</Button>
          <Button
            onClick={handleSaveNode}
            variant="contained"
            color="primary"
            disabled={!nodeName || !nodeUrl}
          >
            Save
          </Button>
        </DialogActions>
      </Dialog>

      {/* Feedback Snackbar */}
      <Snackbar
        open={snackbar.open}
        autoHideDuration={4000}
        onClose={handleCloseSnackbar}
      >
        <Alert 
          onClose={handleCloseSnackbar} 
          severity={snackbar.severity}
          variant="filled"
        >
          {snackbar.message}
        </Alert>
      </Snackbar>
    </>
  );
};

export default NodeSelector; 