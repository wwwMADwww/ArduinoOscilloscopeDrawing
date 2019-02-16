// #include <stdio.h>
// #include "ilda/IldaStreamReader.h"
// #include "testpattern.h"
// 
// 
// void qwe()
// {
// 
//     IldaFrame frame;
//     char recordBuf[IldaStreamReader::RECORD_BUFFER_SIZE] = {};
// 
//     int code = IldaStreamReader::ERROR_NOT_ENOUGHT_DATA;
// 
//     IldaStreamReader reader;
// 
//     int testPatternPos = 0;
//     int testPatternSize = 672;
//     int bufSize = 656;
//      
//     while (true)
//     {
// 
//         if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
//         {
// 
//             if (testPatternPos == testPatternSize)
//             {
//                 //printf("end of external data break;\n");
//                 break;
//             } 
// 
//             code = reader.SetRawData((testPattern + testPatternPos), bufSize);
//             //printf("SetRawData %d, %d %d\n", code, testPatternPos, bufSize); 
//             if (testPatternPos + bufSize >= testPatternSize)
//                 bufSize = testPatternSize - testPatternPos;
//             testPatternPos += bufSize;
//         }
// 
//         code = reader.Read();
// 
//         //printf("read %d\n", code);
// 
//         if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
//             continue;
// 
//         int elementType = IldaStreamReader::ELEMENTTYPE_NONE;
// 
//         code = reader.GetLastReadElementType(&elementType);
//         //printf("GetLastReadElementType %d, %d\n", code, elementType);
// 
//         if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
//             continue;
// 
//         if (elementType == IldaStreamReader::ELEMENTTYPE_FRAME)
//         {
// 
//             code = reader.GetFrame(&frame);
//             if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
//                 continue;
// 
//             //printf("GetFrame %d, %d %d %d %d %s %s\n", code, 
//             //    frame.formatCode, frame.frameNumber, frame.totalFrames, frame.numberOfRecords & 0xFFFF, 
//             //    frame.frameName,  frame.companyName);
// 
//             if (frame.numberOfRecords == 0)
//             {
//                 //printf("frame.numberOfRecords == 0 break;\n");
//                 break;
//             }
//         }
//         else if (elementType == IldaStreamReader::ELEMENTTYPE_RECORD)
//         {
//             code = reader.GetRecord(recordBuf);
//             //printf("GetRecord %d\n", code);
//             if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
//                 continue;
// 
//             switch (frame.formatCode)
//             {
//                 case 0: 
//                 {
//                     IldaRecordF0 * record = (IldaRecordF0 *) recordBuf;
//                     //printf("IldaRecordF0 %d %d %d, %d, %d\n", 
//                     //    record->x, record->y, record->z, 
//                     //    record->statusCode, record->colorIndex);
//                     break;
//                 }
// 
//                 case 1: 
//                 {
//                     IldaRecordF1 * record = (IldaRecordF1 *) recordBuf;
//                     //printf("IldaRecordF1 %d %d, %d, %d\n", 
//                     //    record->x, record->y, 
//                     //    record->statusCode, record->colorIndex);
//                     break;
//                 }
// 
//                 case 2: 
//                 {
//                     IldaRecordF2 * record = (IldaRecordF2 *) recordBuf;
//                     //printf("IldaRecordF2 %d %d %d\n", 
//                     //    record->r, record->g, record->b);
//                     break;
//                 }
// 
//                 case 4:
//                 {
//                     IldaRecordF4 * record = (IldaRecordF4 *) recordBuf;
//                     //printf("IldaRecordF4 %d %d %d, %d, %d %d %d\n", 
//                     //    record->x, record->y, record->z, 
//                     //    record->statusCode,
//                     //    record->r, record->g, record->b);
//                     break;
//                 }
// 
//                 case 5:
//                 {
//                     IldaRecordF5 * record = (IldaRecordF5 *) recordBuf;
//                     //printf("IldaRecordF5 %d %d, %d, %d %d %d\n", 
//                     //    record->x, record->y, 
//                     //    record->statusCode,
//                     //    record->r, record->g, record->b);
//                     printf("%d %d\n", record->x, record->y);
//                     break;
//                 }
//             
//                 //default:
//                     //printf("unknown formatCode: %d\n", frame.formatCode);
// 
//             }
// 
//         }
//         else
//         {
//             //printf("unknown ElementType %d break;\n", elementType);
//             break;
//         }
// 
//     
//     } // /while
// 
//     printf("end\n");
// 
// }
// 
// 
// int main()
// {
//     while(true)
//     {
//         qwe();  
//         printf("q\n");      
//     }
// 
//     return 0;
// }