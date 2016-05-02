//////////////////////////////////////
//
// IoContext.cpp
//
// (see header file)
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "IoContext.h"


void CBufferedFileIO::write(const size_t chunkSize)
{
    size_t writeSize = chunkSize;
    if (m_File.IsOpen() == false)
    {
        if (m_File.Open(m_Name.toStdString(), true) == false)
        {
            throw ArcException("Failed to open output", m_Name.toStdString());
        }
    }

    // buffer may be larger than actual output: write only actual data
    if (writeSize == 0)
    {
        writeSize = m_Buffer.GetCurrentPos();
    }
    if (m_File.Write(m_Buffer.GetBegin(), writeSize) == false)
    {
        throw ArcException("Failed to write output", m_Name.toStdString());
    }

    // for pre-chunk writing, prepare for next
    m_Buffer.MoveToBegin(writeSize);
    if (m_File.Flush() == false)
    {
        throw ArcException("Failed to flush output", m_Name.toStdString());
    }
}
