import axios from 'axios';

// Default node configurations with direct API URLs
const DEFAULT_NODES = [
  { name: 'Local Node 1', url: 'http://localhost:8080/api', isActive: true },
  { name: 'Local Node 2', url: 'http://localhost:8081/api', isActive: false },
  { name: 'Local Node 3', url: 'http://localhost:8085/api', isActive: false },
  { name: 'Local Wallet', url: 'http://localhost:8090/api', isActive: false },
];

// Storage key for persisting nodes in localStorage
const STORAGE_KEY = 'blockchain_nodes';

class NodeConnectionManager {
  constructor() {
    this.nodes = this.loadNodes();
    this.activeNode = this.getActiveNode();
    this.listeners = [];
  }

  // Load nodes from localStorage or use defaults
  loadNodes() {
    try {
      const storedNodes = localStorage.getItem(STORAGE_KEY);
      if (storedNodes) {
        const nodes = JSON.parse(storedNodes);
        if (Array.isArray(nodes) && nodes.length > 0) {
          return nodes;
        }
      }
    } catch (error) {
      console.error('Error loading nodes from storage:', error);
    }
    return [...DEFAULT_NODES];
  }

  // Save nodes to localStorage
  saveNodes() {
    try {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(this.nodes));
    } catch (error) {
      console.error('Error saving nodes to storage:', error);
    }
  }

  // Get the currently active node
  getActiveNode() {
    const active = this.nodes.find(node => node.isActive);
    return active || this.nodes[0];
  }

  // Set active node by index
  setActiveNode(index) {
    if (index >= 0 && index < this.nodes.length) {
      // Update active state for all nodes
      this.nodes.forEach((node, i) => {
        node.isActive = i === index;
      });
      this.activeNode = this.nodes[index];
      this.saveNodes();
      
      // Notify listeners
      this.notifyListeners();
      return true;
    }
    return false;
  }

  // Add a new node
  addNode(name, url) {
    if (!name || !url) return false;
    
    // Check if URL already exists
    if (this.nodes.some(node => node.url === url)) {
      return false;
    }
    
    const newNode = { name, url, isActive: false };
    this.nodes.push(newNode);
    this.saveNodes();
    
    return true;
  }

  // Remove a node by index
  removeNode(index) {
    if (index >= 0 && index < this.nodes.length) {
      const wasActive = this.nodes[index].isActive;
      this.nodes.splice(index, 1);
      
      // If we removed the active node, set first node as active
      if (wasActive && this.nodes.length > 0) {
        this.nodes[0].isActive = true;
        this.activeNode = this.nodes[0];
      }
      
      this.saveNodes();
      
      // Notify listeners if active node changed
      if (wasActive) {
        this.notifyListeners();
      }
      
      return true;
    }
    return false;
  }

  // Edit a node by index
  editNode(index, name, url) {
    if (index >= 0 && index < this.nodes.length && name && url) {
      this.nodes[index].name = name;
      this.nodes[index].url = url;
      
      // Update active node reference if needed
      if (this.nodes[index].isActive) {
        this.activeNode = this.nodes[index];
      }
      
      this.saveNodes();
      return true;
    }
    return false;
  }

  // Test connection to a node
  async testConnection(url) {
    try {
      console.log(`Testing connection to ${url}/statistics`);
      const response = await axios.get(`${url}/statistics`, { 
        timeout: 5000,
        headers: {
          'Accept': 'application/json'
        }
      });
      console.log(`Connection test result: ${response.status}`);
      return response.status === 200;
    } catch (error) {
      console.error('Connection test failed:', error);
      return false;
    }
  }

  // Get all nodes
  getNodes() {
    return [...this.nodes];
  }

  // Add change listener
  addListener(callback) {
    if (typeof callback === 'function') {
      this.listeners.push(callback);
    }
  }

  // Remove change listener
  removeListener(callback) {
    this.listeners = this.listeners.filter(listener => listener !== callback);
  }

  // Notify all listeners
  notifyListeners() {
    this.listeners.forEach(listener => {
      try {
        listener(this.activeNode);
      } catch (error) {
        console.error('Error in node connection listener:', error);
      }
    });
  }

  // Reset to default nodes
  resetToDefaults() {
    try {
      // First completely clear the nodes from localStorage
      localStorage.removeItem(STORAGE_KEY);
      console.log('Cleared stored nodes from localStorage');
    } catch (e) {
      console.error('Error clearing localStorage:', e);
    }
    
    // Reset to default nodes
    this.nodes = [...DEFAULT_NODES];
    this.activeNode = this.getActiveNode();
    this.saveNodes();
    this.notifyListeners();
    console.log('Reset to default nodes:', this.nodes);
  }
}

// Create and export a singleton instance
const nodeManager = new NodeConnectionManager();
export default nodeManager; 