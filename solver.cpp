#include <iostream>
#include <stdint.h>
#include <set>
#include <vector>
#include <fstream>
using namespace std;

#define LEFT 0
#define TOP 1
#define RIGHT 2
#define BOTTOM 3

#define UNUSED -1

class State{
	vector<int> m_cells;

	int m_usedBlocks;

public:
	State();
	void initialize(int length);
	void print();
	void getNextStates(vector<State>*states);
	vector<int>*getCells();
	int isFull();
	void printScore();
};

int State::isFull(){
	return m_usedBlocks==(int)m_cells.size();
}

vector<int>*State::getCells(){
	return &m_cells;
}

State::State(){
}

void State::getNextStates(vector<State>*states){
	vector<int> availableCells;
	vector<int> availableBlocks;

	set<int> usedBlocks;
	for(int i=0;i<(int)m_cells.size();i++){
		int block=m_cells[i];
		if(block==UNUSED){
			availableCells.push_back(i);
		}else{
			usedBlocks.insert(block);
		}
	} 

	for(int i=0;i<(int)m_cells.size();i++){
		if(usedBlocks.count(i)==0)
			availableBlocks.push_back(i);
	}

	for(int i=0;i<(int)availableBlocks.size();i++){
		for(int j=0;j<(int)availableCells.size();j++){
			State newState;
			newState=*this;
			newState.m_cells[availableCells[j]]=availableBlocks[i];

			newState.m_usedBlocks++;
			states->push_back(newState);
		}
	}
}

void State::printScore(){
	cout<<m_usedBlocks<<"/"<<m_cells.size()<<" ";
	double ratio=100.0*m_usedBlocks/m_cells.size();
	cout<<ratio<<"%"<<endl;
}

void State::print(){
	printScore();
	for(int i=0;i<(int)m_cells.size();i++)
		cout<<" "<<m_cells[i];
	cout<<endl;
}

void State::initialize(int length){
	for(int i=0;i<length;i++)
		m_cells.push_back(UNUSED);

	m_usedBlocks=0;
}

class Block{
	int m_left;
	int m_top;
	int m_right;
	int m_bottom;

public:
	Block();
	void loadFromFile(ifstream*f);
	
	int getValue(int side);

	void print();
};

void Block::print(){
	cout<<m_left<<" "<<m_top<<" "<<m_right<<" "<<m_bottom<<endl;
}

int Block::getValue(int side){
	switch(side){
		case LEFT:
			return m_left;
		case RIGHT:
			return m_right;
		case TOP:
			return m_top;
		case BOTTOM:
			return m_bottom;
	}
	return UNUSED;
}

Block::Block(){
}

void Block::loadFromFile(ifstream*f){
	*f>>m_left>>m_top>>m_right>>m_bottom;
}

class Board{
	uint64_t m_timer;
	int m_step;

	int m_width;
	int m_height;

	vector<Block> m_blocks;

	bool solveState(State*state);
	bool isValid(State*state);
	bool isValidSide(State*state,int cell,int side,int cell1,int side1);
	bool isValidSideImplementation(State*state,int cell,int side,int cell1,int side1);

	void printState(State*state);
public:
	Board();

	void loadFromFile(ifstream*file);

	void solve();
};

void Board::solve(){
	State state;
	state.initialize(m_width*m_height);

	m_timer=0;
	m_step=100000;

	solveState(&state);
}

bool Board::isValid(State*state){
	for(int i=0;i<(int)state->getCells()->size();i++){
		int left=i-1;

		if(i%m_width!=0 && !isValidSide(state,i,LEFT,left,RIGHT))
			return false;

		int right=i+1;

		if(right%m_width!=0 && !isValidSide(state,i,RIGHT,right,LEFT))
			return false;

		int top=i-m_width;

		if(!isValidSide(state,i,TOP,top,BOTTOM))
			return false;

		int bottom=i+m_width;

		if(!isValidSide(state,i,BOTTOM,bottom,TOP))
			return false;
	}

	return true;
}

bool Board::isValidSide(State*state,int cell,int side,int cell1,int side1){
	bool flag=isValidSideImplementation(state,cell,side,cell1,side1);

	if(!flag){
		//cout<<"cell "<<cell<<" side "<<side<<" cell "<<cell1<<" side "<<side1<<" not valid"<<endl;
	}

	return flag;
}

bool Board::isValidSideImplementation(State*state,int cell,int side,int cell1,int side1){
	if(cell<0)
		return true;

	if(cell1<0)
		return true;

	int n=state->getCells()->size();
	
	if(cell>=n)
		return true;

	if(cell1>=n)
		return true;

	int block=state->getCells()->at(cell);

	if(block==UNUSED)
		return true;

	int block1=state->getCells()->at(cell1);

	if(block1==UNUSED)
		return true;

	//cout<<"cell "<<cell<<" has block "<<block<<endl;
	//cout<<"cell1 "<<cell1<<" has block1 "<<block1<<endl;

	return m_blocks[block].getValue(side)==m_blocks[block1].getValue(side1);
}

void Board::printState(State*state){
	//cout<<"***"<<endl;

	//state->print();
	for(int i=0;i<(int)state->getCells()->size();i++){
		int block=state->getCells()->at(i);
		if(block==UNUSED){
			cout<<endl;
		}else{
			m_blocks[block].print();
		}
	}
}

bool Board::solveState(State*state){

	if(m_timer%m_step==0){
		cout<<m_timer<<" ";
		state->printScore();
	}

	m_timer++;

	if(!isValid(state))
		return false;

	//state->printScore();

	if(state->isFull()){
		cout<<"found solution after checking "<<m_timer<<" states"<<endl;
		printState(state);
		return true;
	}

	vector<State> nextStates;
	state->getNextStates(&nextStates);

	for(int i=0;i<(int)nextStates.size();i++){
		if(solveState(&(nextStates[i])))
			return true;
	}

	return false;
}

Board::Board(){
}

void Board::loadFromFile(ifstream*f){
	*f>>m_width>>m_height;

	for(int i=0;i<m_height;i++){
		for(int j=0;j<m_width;j++){
			Block block;
			block.loadFromFile(f);
			m_blocks.push_back(block);
		}
	}
}


int main(int argc,char**argv){
	ifstream f(argv[1]);
	Board board;
	board.loadFromFile(&f);
	f.close();

	board.solve();

	

	return 0;
}
