#!/bin/bash
# LINA - Run with API Mode Support
# Allows testing both local and API modes

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo -e "${BLUE}═══════════════════════════════════════════════════${NC}"
echo -e "${BLUE}    LINA - AI Network Analyzer Launcher${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════${NC}\n"

# Check Python
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}❌ Python 3 not found${NC}"
    exit 1
fi

# Create/activate virtual environment
if [ ! -d ".venv" ]; then
    echo -e "${YELLOW}📦 Creating virtual environment...${NC}"
    python3 -m venv .venv
fi

source .venv/bin/activate

# Install/update requirements
echo -e "${YELLOW}📚 Installing/updating requirements...${NC}"
pip install -q -r desktop_py/requirements.txt

echo -e "\n${BLUE}═══════════════════════════════════════════════════${NC}"
echo -e "${BLUE}    Select Mode${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════${NC}\n"

echo "1. Local Mode (Mistrallite - Default, Free, Private)"
echo "2. API Mode (OpenAI ChatGPT - Fast, Cloud-based)"
echo "3. Demo Mode (Mock API - Testing without API key)"
echo -e "\n${YELLOW}Select mode (1-3):${NC} "
read -r mode

case $mode in
    1)
        echo -e "\n${GREEN}✓ Running in LOCAL MODE${NC}\n"
        unset LINA_USE_API
        unset LINA_API_KEY
        unset LINA_API_MOCK_MODE
        python3 desktop_py/main.py
        ;;
    2)
        echo -e "\n${BLUE}API MODE SELECTED${NC}"
        echo -e "${YELLOW}Enter your OpenAI API key (sk-...):${NC} "
        read -r api_key
        
        if [ -z "$api_key" ]; then
            echo -e "${RED}❌ API key cannot be empty${NC}"
            exit 1
        fi
        
        echo -e "\n${GREEN}✓ Running in API MODE${NC}"
        echo -e "   Using API Key: ${api_key:0:10}...${api_key: -5}\n"
        
        export LINA_USE_API=1
        export LINA_API_KEY="$api_key"
        unset LINA_API_MOCK_MODE
        python3 desktop_py/main.py
        ;;
    3)
        echo -e "\n${GREEN}✓ Running in DEMO MODE (Mock API)${NC}\n"
        export LINA_USE_API=1
        export LINA_API_KEY="sk-demo-key"
        export LINA_API_MOCK_MODE=1
        python3 desktop_py/main.py
        ;;
    *)
        echo -e "${RED}❌ Invalid selection${NC}"
        exit 1
        ;;
esac
