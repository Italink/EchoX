using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using NPSMLib;

namespace QSmtcServer{
    class QSmtcServer
    {
        static void SendMessgae(ref NetworkStream stream, JObject json)
        {
            try
            {
                //Console.WriteLine("Send Msg {0}", json.ToString());
                var jsonString = JsonConvert.SerializeObject(json);
                byte[] jsonBuffer = Encoding.UTF8.GetBytes(jsonString);
                byte[] lengthBuffer = System.BitConverter.GetBytes(jsonBuffer.Length);
                stream.WriteAsync(lengthBuffer, 0, lengthBuffer.Length);
                stream.WriteAsync(jsonBuffer, 0, jsonBuffer.Length);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        static void HandleMessage(ref NetworkStream stream, ref NPSMLib.NowPlayingSessionManager manager)
        {
            if (!stream.DataAvailable)
                return;
            byte[] buffer = new byte[4];
            stream.ReadAsync(buffer, 0, buffer.Length).Wait();
            int dataLength = System.BitConverter.ToInt32(buffer, 0);
            if (dataLength <= 0)
                return;
            try
            {
                byte[] jsonData = new byte[dataLength];
                stream.ReadAsync(jsonData, 0, jsonData.Length).Wait();
                string jsonStr = Encoding.ASCII.GetString(jsonData);
                var json = JsonConvert.DeserializeObject<Dictionary<String, Object>>(jsonStr);
                var command = json["Command"].ToString();
                if (command.Equals("GetMediaPlaybackDataSourceInfo"))
                {
                    JObject jsonInfo = new JObject();
                    var info = manager.CurrentSession.ActivateMediaPlaybackDataSource().GetMediaObjectInfo();
                    jsonInfo["Reply"] = "MediaPlaybackDataSourceInfo";
                    jsonInfo["SourceAppId"] = manager.CurrentSession.SourceAppId;
                    jsonInfo["SourceDeviceId"] = manager.CurrentSession.SourceDeviceId;
                    jsonInfo["RenderDeviceId"] = manager.CurrentSession.RenderDeviceId;
                    jsonInfo["MediaClassPrimaryID"] = info.MediaClassPrimaryID;
                    jsonInfo["Title"] = info.Title;
                    jsonInfo["Artist"] = info.Artist;
                    jsonInfo["Subtitle"] = info.Subtitle;
                    jsonInfo["Genres"] = string.Join("|", info.Genres);
                    jsonInfo["TrackNumber"] = info.TrackNumber;
                    jsonInfo["AlbumTitle"] = info.AlbumTitle;
                    jsonInfo["AlbumArtist"] = info.AlbumArtist;
                    jsonInfo["AlbumTrackCount"] = info.AlbumTrackCount;
                    var thumbnailStream = manager.CurrentSession.ActivateMediaPlaybackDataSource().GetThumbnailStream();
                    if (thumbnailStream != null)
                    {
                        var path = Path.Combine(Path.GetTempPath(), info.Title +".png");
                        if (!File.Exists(path))
                        {
                            var file = File.Create(path);
                            thumbnailStream.CopyTo(file);
                            thumbnailStream.Close();
                            file.Close();
                            thumbnailStream.Dispose();
                            file.Dispose();
                        }
                        jsonInfo["Thumbnail"] = path;
                    }

                    SendMessgae(ref stream, jsonInfo);
                }
                else if (command.Equals("GetMediaPlaybackInfo"))
                {
                    JObject jsonInfo = new JObject();
                    var info = manager.CurrentSession.ActivateMediaPlaybackDataSource().GetMediaPlaybackInfo();
                    jsonInfo["Reply"] = "MediaPlaybackInfo";
                    jsonInfo["PlaybackState"] = info.PlaybackState.ToString();
                    jsonInfo["PlaybackCaps"] = info.PlaybackCaps.ToString();
                    jsonInfo["PlaybackMode"] = info.PlaybackMode.ToString();
                    jsonInfo["PlaybackRate"] = info.PlaybackRate;
                    jsonInfo["RepeatMode"] = info.RepeatMode.ToString();
                    jsonInfo["LastPlayingFileTime"] = info.LastPlayingFileTime;
                    SendMessgae(ref stream, jsonInfo);
                }
                else if (command.Equals("GetMediaTimelineProperties"))
                {
                    JObject jsonInfo = new JObject();
                    var info = manager.CurrentSession.ActivateMediaPlaybackDataSource().GetMediaTimelineProperties();
                    jsonInfo["Reply"] = "MediaTimelineProperties";
                    jsonInfo["Position"] = info.Position;
                    jsonInfo["StartTime"] = info.StartTime;
                    jsonInfo["EndTime"] = info.EndTime;
                    jsonInfo["MinSeekTime"] = info.MinSeekTime;
                    jsonInfo["MaxSeekTime"] = info.MaxSeekTime;
                    jsonInfo["PositionSetFileTime"] = info.PositionSetFileTime;
                    SendMessgae(ref stream, jsonInfo);
                }
                else if (command.Equals("SendCommand"))
                {
                    var commandID = (long?)json["CommandID"];
                    manager.CurrentSession.ActivateMediaPlaybackDataSource().SendMediaPlaybackCommand((MediaPlaybackCommands)commandID);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        delegate void DataChangedEvent(MediaObjectInfo mediaObjectInfo);
        static string mLastTitle;
        static void TickLoop(ref TcpClient client) {
            Console.WriteLine("Smtc Connect Success");
            var stream = client.GetStream();

            DataChangedEvent changedEvent = delegate (MediaObjectInfo mediaObjectInfo)
            {
                if (mLastTitle != mediaObjectInfo.Title)
                {
                    //Console.WriteLine("Smtc Connect Changed {0} -> {1}", mLastTitle, mediaObjectInfo.Title);
                    mLastTitle = mediaObjectInfo.Title;
                    JObject json = new JObject();
                    json["Reply"] = "MediaPlaybackDataChanged";
                    SendMessgae(ref stream, json);
                }
            };

            var eventHander = new EventHandler<MediaPlaybackDataChangedArgs>(delegate (Object o, MediaPlaybackDataChangedArgs a)
            {
                changedEvent(a.MediaPlaybackDataSource.GetMediaObjectInfo());
            });

            NPSMLib.NowPlayingSessionManager manager = new NPSMLib.NowPlayingSessionManager();
            try
            {
                while (client.Connected)
                {
                    if (manager.CurrentSession != null && manager.CurrentSession.ActivateMediaPlaybackDataSource() != null)
                    {
                        changedEvent(manager.CurrentSession.ActivateMediaPlaybackDataSource().GetMediaObjectInfo());
                        manager.CurrentSession.ActivateMediaPlaybackDataSource().MediaPlaybackDataChanged += eventHander;
                        HandleMessage(ref stream, ref manager);
                    }
                }
                client.Dispose();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
            finally
            {
                stream.Close();
            }
        }
        static async Task Main(string[] args) {
            ThreadPool.SetMaxThreads(4, 8);
            int port = 40529;
            IPAddress localAddr = IPAddress.Parse("127.0.0.1");
            TcpListener server = new TcpListener(localAddr, port);
            server.Start();
            var client = await server.AcceptTcpClientAsync();
            TickLoop(ref client);
            server.Stop();

        }
    }
}
