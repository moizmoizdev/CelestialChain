import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { ThemeProvider, CssBaseline } from '@mui/material';
import theme from './theme';
import MainLayout from './components/Layout/MainLayout';

// Import pages (will create these next)
import Dashboard from './pages/Dashboard';
import Blockchain from './pages/Blockchain';
import Mempool from './pages/Mempool';
import Mining from './pages/Mining';
import Transactions from './pages/Transactions';
import Wallet from './pages/Wallet';
import Explorer from './pages/Explorer';
import Peers from './pages/Peers';
import SyncNetwork from './pages/SyncNetwork';
import Settings from './pages/Settings';
import NotFound from './pages/NotFound';
import ExplorerBlockView from './pages/ExplorerBlockView';
import ExplorerTransactionView from './pages/ExplorerTransactionView';
import ExplorerAddressView from './pages/ExplorerAddressView';
function App() {
  return (
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Router>
        <MainLayout>
          <Routes>
            <Route path="/" element={<Dashboard />} />
            <Route path="/blockchain" element={<Blockchain />} />
            <Route path="/mempool" element={<Mempool />} />
            <Route path="/mine" element={<Mining />} />
            <Route path="/transactions" element={<Transactions />} />
            <Route path="/wallet" element={<Wallet />} />
            <Route path="/explorer" element={<Explorer />} />
            <Route path="/explorer/block/:id" element={<ExplorerBlockView />} />
            <Route path="/explorer/transaction/:hash" element={<ExplorerTransactionView />} />
            <Route path="/explorer/address/:address" element={<ExplorerAddressView />} />
            <Route path="/peers" element={<Peers />} />
            <Route path="/sync" element={<SyncNetwork />} />
            <Route path="/settings" element={<Settings />} />
            <Route path="*" element={<NotFound />} />
          </Routes>
        </MainLayout>
      </Router>
    </ThemeProvider>
  );
}

export default App;
