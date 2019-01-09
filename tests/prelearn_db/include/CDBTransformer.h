#ifndef CDBTRANSFORMER_H
#define CDBTRANSFORMER_H

#include "../globle.h"
#include "IDBLoader.h"
#include "IDBSaver.h"

class DLL_EXPORT CDBTransformer
{
    public:
        CDBTransformer();
        virtual ~CDBTransformer();

        bool Transform();

        void SetLoader(IDBLoader *pLoader){ m_pLoader = pLoader; }
        void SetSaver(IDBSaver *pSaver){ m_pSaver = pSaver; }
    protected:
    private:
        IDBLoader *m_pLoader;
        IDBSaver *m_pSaver;
};

#endif // CDBTRANSFORMER_H
