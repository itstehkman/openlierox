#include "posspd_replicator.h"

#include "netstream.h"
#include <iostream>
#include "util/vec.h"
#include "encoding.h"
#include <utility>
using std::cerr;
using std::endl;

PosSpdReplicator::PosSpdReplicator(Net_ReplicatorSetup *_setup,
	CVec *pos, CVec *spd, Encoding::VectorEncoding& encoding_,
	Encoding::DiffVectorEncoding& diffEncoding_)
: Net_ReplicatorBasic(_setup)
, m_posPtr(pos)
, m_spdPtr(spd)
, m_repCount(0)
, encoding(encoding_)
, diffEncoding(diffEncoding_)
{
	m_flags |= Net_REPLICATOR_INITIALIZED;
}

bool PosSpdReplicator::checkState()
{
#ifdef COMPACT_FLOATS
	std::pair<long, long> v = encoding.quantize(*m_posPtr);
	bool s = (m_oldPos.first != v.first || m_oldPos.second != v.second);
	m_oldPos = v;
#else
	bool s = (( m_posCmp.x != m_posPtr->x ) || ( m_posCmp.y != m_posPtr->y));
	m_posCmp = *m_posPtr;
#endif
	return s;
}

void PosSpdReplicator::packData(Net_BitStream *_stream)
{
#ifdef COMPACT_FLOATS
/*
	dynamic_bitset<> n = gusGame.level().vectorEncoding.encode(*m_posPtr);
	Encoding::writeBitset(*_stream, n);
*/
	encoding.encode(*_stream, *m_posPtr);
#else
	_stream->addFloat(m_posPtr->x,32);
	_stream->addFloat(m_posPtr->y,32);
#endif

	if ( m_repCount >= speedRepTime )
	{
		m_repCount = 0;
		_stream->addBool(true);
/*
		_stream->addFloat(m_spdPtr->x,speedPrec);
		_stream->addFloat(m_spdPtr->y,speedPrec);*/
		diffEncoding.encode(*_stream, *m_spdPtr);
	}else
	{
		++m_repCount;
		_stream->addBool(false);
	}

}

void PosSpdReplicator::unpackData(Net_BitStream *_stream, bool _store)
{
	if (_store)
	{
#ifdef COMPACT_FLOATS
/*
		dynamic_bitset<> n = Encoding::readBitset(*_stream, gusGame.level().vectorEncoding.bits);
		*m_posPtr = gusGame.level().vectorEncoding.decode<Vec>(n);*/
		*m_posPtr = CVec(encoding.decode<Vec>(*_stream));
#else
		m_posPtr->x = _stream->getFloat(32);
		m_posPtr->y = _stream->getFloat(32);
#endif

		if ( _stream->getBool() )
		{
			/*
			m_spdPtr->x = _stream->getFloat(speedPrec);
			m_spdPtr->y = _stream->getFloat(speedPrec);*/
			*m_spdPtr = CVec(diffEncoding.decode<Vec>(*_stream));
		}

	}
	else 
	{
#ifdef COMPACT_FLOATS
		//Encoding::readBitset(*_stream, gusGame.level().vectorEncoding.bits);
		encoding.decode<Vec>(*_stream);
#else
		_stream->getFloat(32);
		_stream->getFloat(32);
#endif

		if ( _stream->getBool() )
		{
			/*
			_stream->getFloat(speedPrec);
			_stream->getFloat(speedPrec);*/
			*m_spdPtr = CVec(diffEncoding.decode<Vec>(*_stream));
		}
	}
}

void* PosSpdReplicator::peekData()
{
	assert(getPeekStream());
	Vec *retVec = new Vec;
#ifdef COMPACT_FLOATS
/*
	dynamic_bitset<> n = Encoding::readBitset(*getPeekStream(), gusGame.level().vectorEncoding.bits);
	*retVec = gusGame.level().vectorEncoding.decode<Vec>(n);*/
	*retVec = encoding.decode<Vec>(*getPeekStream());
#else
	retVec->x = getPeekStream()->getFloat(32);
	retVec->y = getPeekStream()->getFloat(32);
#endif
	peekDataStore(retVec);
	
	return (void*) retVec;
}

void PosSpdReplicator::clearPeekData()
{
	Vec *buf = (Vec*) peekDataRetrieve();
	delete buf;
};

/*Vec VectorReplicator::getLastUpdate()
{
	return m_posCmp;
}*/
