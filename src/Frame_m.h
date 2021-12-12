//
// Generated file, do not edit! Created by nedtool 5.7 from Frame.msg.
//

#ifndef __FRAME_M_H
#define __FRAME_M_H

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0507
#if (MSGC_VERSION != OMNETPP_VERSION)
#error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// cplusplus {{
typedef std::pair<int, unsigned int> Header;
// first is MessageID, second is its timestamp
// }}

/**
 * Class generated from <tt>Frame.msg:26</tt> by nedtool.
 * <pre>
 * packet Frame
 * {
 *     \@customize(true);  // see the generated C++ header for more info
 *     Header header;
 *     string payload;
 *     char trailer;
 *     bool ack;
 *     int piggyback_id;
 * }
 * </pre>
 *
 * Frame_Base is only useful if it gets subclassed, and Frame is derived from it.
 * The minimum code to be written for Frame is the following:
 *
 * <pre>
 * class Frame : public Frame_Base
 * {
 *   private:
 *     void copy(const Frame& other) { ... }

 *   public:
 *     Frame(const char *name=nullptr, short kind=0) : Frame_Base(name,kind) {}
 *     Frame(const Frame& other) : Frame_Base(other) {copy(other);}
 *     Frame& operator=(const Frame& other) {if (this==&other) return *this; Frame_Base::operator=(other); copy(other); return *this;}
 *     virtual Frame *dup() const override {return new Frame(*this);}
 *     // ADD CODE HERE to redefine and implement pure virtual functions from Frame_Base
 * };
 * </pre>
 *
 * The following should go into a .cc (.cpp) file:
 *
 * <pre>
 * Register_Class(Frame)
 * </pre>
 */
class Frame_Base : public ::omnetpp::cPacket
{
protected:
  Header header;
  ::omnetpp::opp_string payload;
  char trailer;
  bool ack;
  int piggyback_id;

private:
  void copy(const Frame_Base &other);

protected:
  // protected and unimplemented operator==(), to prevent accidental usage
  bool operator==(const Frame_Base &);
  // make constructors protected to avoid instantiation
  Frame_Base(const char *name = nullptr, short kind = 0);
  Frame_Base(const Frame_Base &other);
  // make assignment operator protected to force the user override it
  Frame_Base &operator=(const Frame_Base &other);

public:
  virtual ~Frame_Base();
  Frame_Base(const Header &header, const char *payload, char trailer, bool ack, int piggyback_id);
  // virtual Frame_Base *dup() const override { throw omnetpp::cRuntimeError("You forgot to manually add a dup() function to class Frame"); }
  virtual Frame_Base *dup() const override { return new Frame_Base(*this); }
  virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
  virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

  // field getter/setter methods
  virtual Header &getHeader();
  virtual const Header &getHeader() const { return const_cast<Frame_Base *>(this)->getHeader(); }
  virtual void setHeader(const Header &header);
  virtual const char *getPayload() const;
  virtual void setPayload(const char *payload);
  virtual char getTrailer() const;
  virtual void setTrailer(char trailer);
  virtual bool getAck() const;
  virtual void setAck(bool ack);
  virtual int getPiggyback_id() const;
  virtual void setPiggyback_id(int piggyback_id);

  // Added by Ziyad
  // virtual const char *getDisplayString() const override;
};

#endif // ifndef __FRAME_M_H
